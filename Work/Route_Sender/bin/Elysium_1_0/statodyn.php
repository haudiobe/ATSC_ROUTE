<?php
error_reporting(null);
/*
 * DASH Live Emulation Script
 *
 * {filename}.mpd.php
 * front end mpd script..
 *
 * Usage directions:
 *
 * copy this script into the directory on the web server where your
 * DASH MPD is (say your MPD is called X.mpd)
 * call this file X.mpd.php
 * point your DASH client to the URL ending in X.mpd.php
 *
 * This code has been tested with Apache 2.2.22 / PHP 5.3.10 (Unix)
 * It was intended to not be dependent on any Unix-specific facilities.
 *
 * Kevin Fall
 * Copyright (c) Kevin Fall 2012.  All rights reserved.
 *
 * Further development by Waqar Zia
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     (1) Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 *
 *     (2)The name of the author may not be used to
 *     endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * started 26-Feb-2012 / update 21-Mar-2012 / updated 25-May-2012
 * updated Aug 9 2012 for constant AST across MPD updates
 * updated Aug 13 2012 to pass AST on initial invocation properly
 * updated Aug 22 2012 to crush some bugs found during testing (incl infinite setting for TSBD, and AST items)
 * updated Aug 23 2012 with some significant restructuring and support for publishTime
 * updated Aug 31 2012 to deal with termination of live event (via not modifying mediaPresentationDuration)
 * kfall@qualcomm.com
 */

const DATE_TIME_FORMAT = DATE_W3C;	/* datetime format, specified by DASH (don't change this) */
const DEFAULT_AST = "now";		/* "now" or absolute date for DASH AvailabilityStartTime */
const DEFAULT_AET = "none";		/* DASH AvailabilityEndTime express as a DateInterval spec, relative to now */
const DEFAULT_MUP = "PT597S";		/* DASH MinimumUpdateInterval */
const DEFAULT_TSBD = "infinite";	/* DASH TimeShiftBufferDepth in DateInterval notation or 'infinite' */

const DEFAULT_SPD = "PT1S";		/* DASH SuggestedPresentationDelay in DateInterval format */
const DEFAULT_TZ = "UTC";		/* timezone, as specified by DASH standard */
const UNIX_ZERO_TS = "@0";		/* No time == UNIX epoch */

const NumPattern = '/\$Number\$/';		/* find $Number$ type patterns in URL */
const NumReplace = '^\$Number$1\$^';		/* sentinels '^' surround number */
const TimePattern = '/\$Time(.*)\$/';		/* find $Time$ type patterns in URL */
const TimeReplace = '=\$Time$1\$=';		/* sentinels '=' surround time */

const Profiles=					/* profiles to output in MPD */
	'urn:mpeg:dash:profile:isoff-live:2011';
const DotMPDHack = 1;				/* append ".mpd" to all MPD URLs (certain clients) */

$tz = new DateTimeZone(DEFAULT_TZ);	/* the local timezone, mostly required by PHP */

/*
 * first, some debugging stuff
 * these must be set to zero for testing with actual DASH client applications
 * they are not constants, because e.g. "debug=1" can be included in query string
 * which sets these
 */

$DEBUG = 0;	/* general debugging */
$SEGDEBUG = 0;	/* segment processing time ranges */

 $REDIRECTTOFILE = 0; /*Re-direct stdout to txt file*/

 $DUMPMPD = 1; /*In addition of processing query argument mpd, just dump mpd*/

if($REDIRECTTOFILE)
{	

	function ob_file_callback($buffer)
	{
	  global $ob_file;
	  fwrite($ob_file,$buffer);
	}

    clearstatcache();

	$ob_file = fopen('log.txt','w') or die("can't open file");

	ob_start('ob_file_callback');
}

/*Send out CORS header*/
function corsHeader()
{
	header("Access-Control-Allow-Headers: origin,range");
	header("Access-Control-Expose-Headers: Server,range");
	header("Access-Control-Allow-Methods: GET, HEAD, OPTIONS");
	header("Access-Control-Allow-Origin: *");
}

function sxmlAppend(SimpleXMLElement $to, SimpleXMLElement $from) {
    $toDom = dom_import_simplexml($to);
    $fromDom = dom_import_simplexml($from);
    $toDom->appendChild($toDom->ownerDocument->importNode($fromDom, true));
}

/*
 * given the string naming a variable, print the variable / val pair
 */
function printexists($v) {
	echo '$_SERVER[' . $v . ']: ';
	echo (isset($_SERVER[$v])) ? $_SERVER[$v] : "NOT DEFINED";
	echo "<br />\n";
}

/*
 * for clients which require MPD URLs to end in '.mpd'
 * 	ensures URI ends in ".mpd"
 *	by adding dummy element "hack"
 */
function mpdhack($str) {
	if (substr($str, -4) === ".mpd")
		return $str; // no need to add anything
	if (DotMPDHack) {
		return $str . "&hack=.mpd"; // append
	} else {
		return $str;
	}
}

/*
 * convert unix TS to an NTP style TS
 * (something the new MPEG DASH spec unfortunately wants)
 */
function ntpts($unixts) {
	/* convert unix timestamp to NTP timestamp */
	$ntpval = $unixts + 2208988800;
	return $ntpval;
}

/*
 * determine the request URI in long format (including name and port)
 * (note $_SERVER['REQUEST_URI'] also includes query string, so don't use that)
 */
function request_uri_long() {
	$self = $_SERVER['PHP_SELF'];	// includes leading "/"
	if ($_SERVER['SERVER_PORT'] != 80)  {
		$uri =
		    'http://'.htmlspecialchars($_SERVER['SERVER_NAME'].':'.$_SERVER['SERVER_PORT'].$self);
	} else {
		$uri = 'http://'.htmlspecialchars($_SERVER['SERVER_NAME'].$self);
	}
	return $uri;
}

 
/*
 * if DEBUG is on, print a bunch of the environment
 * 	debug=1 in query string sets DEBUG
 * 	segdebug=1 in query string sets SEGDEBUG
 */

if (isset($_GET['debug'])) {
	$DEBUG = $_GET['debug'];
}

if (isset($_GET['segdebug'])) {
	$SEGDEBUG = $_GET['segdebug'];
}

if ($DEBUG) {
	echo "<p>\n";
	printexists('QUERY_STRING');
	printexists('DOCUMENT_ROOT');
	printexists('REQUEST_URI');
	printexists('PHP_SELF');
	printexists('SCRIPT_FILENAME');
	printexists('SERVER_NAME');
	printexists('REQUEST_METHOD');
	printexists('SERVER_PORT');

	printexists('REQUEST_TIME');
	printexists('REQUEST_TIME_FLOAT');
	printexists('REQUEST_URL');	// like URI, but w/out query string
	printexists('PATH_INFO');	// after script name but preceding query string
	printexists('DOCUMENT_ROOT');
	printexists('PATH_TRANSLATED');
	printexists('ORIG_PATH_INFO');
	printexists('HTTP_HOST');
	echo "</p>\n";
}

/*
 * Main part
 */

/*
 * formulate the request URI (without query string)
 */

$request_uri = request_uri_long();

/*
 * figure out if this is:
 *	first request for MPD (no type present)
 *	subsequent request for MPD (indicated by ?type=mpd)
 *	or request for a segment (indicated by ?type=seg)
 *
 * if its an initial request for MPD, do a re-direct using a URL with special query string options set
 *    special options in the query string allow to scrawl away a copy of the generated MPD or provide the AST
 * if its a subsequent request for an MPD, re-write the local one to be a live one
 * if its a request for a segment, ensure the segment should be available now (to simulate live setting)
 */

$qstr_is_set = ($_SERVER['QUERY_STRING'] != '');	/* query string set? */
$type_is_set = isset($_GET['type']);

$mpdout_is_set = isset($_GET['mpd']);		/* capture a copy of the generated MPD in this file */
$ast_is_set = isset($_GET['avail_start']);	/* in case AST is provided (e.g. MPD update testing) */
$pt_is_set = isset($_GET['pt']);		/* if publish time is given */

corsHeader();

/* handle initial requests, with or without query string parameters */
if (!$type_is_set) {
	/* first request for MPD: re-direct to ourselves, but with type=XXX and other stuff in query string */
	/* NB: redirects must contain absolute URIs (RFC2616, 14.30) */

	$new_uri = 'Location: ' . $request_uri . '?type=mpd';

	if (!$ast_is_set) {
	
		if (isset($_GET['avail_date']) && isset($_GET['avail_time'])) {

			$rdate = $_GET['avail_date'];
			$rtime = $_GET['avail_time'];
		
			$abdt = $rdate.' '.$rtime;
			$ast = new DateTime($abdt,$tz);	
	
			$new_uri .= '&avail_start=' . $ast->format("U");
				
		}
	}
	else{
		/* set AST to what the script constants say above */
		$ast = new DateTime(DEFAULT_AST, $tz);	/* initializer for availability start time */
		$new_uri .= '&avail_start=' . $ast->format("U");
	}

	if (!$pt_is_set) {
		/* if MPD publish time isn't provided, make it now */
		$new_uri .= '&pt=' . strval(time());
	}

	/* append any other qstr parameters, including mpd= or avail_start= or debug= or pt= */
	if ($qstr_is_set) {
		$new_uri .= "&" . $_SERVER['QUERY_STRING'];
	}
	/* output the redirect */
	header(mpdhack($new_uri));
	exit;
}

/*
 * ok, we have a type requested (not an initial MPD request)
 * see if the type is one we understand
 * types are: mpd, seg
 */

switch ($_GET['type']) {
case 'mpd':
	header("Content-Type: application/xml");
        file_put_contents ( "logFile2.txt" , "Starting debug ...\n" );
	/* incoming request for the MPD (after redirect)  */
	$targetname = $_SERVER['PHP_SELF'];	// path name of myself
	if ($DEBUG) {
		echo "*** PROCESSING MPD REQ>".$targetname."<\n";
	}
	$mpdout = process_mpd($targetname);	// creates new MPD
	echo $mpdout;				// writes it to client
	if ($mpdout_is_set) {
		/* we were asked to drop a copy of the generated MPD */
		/* &mpd=XXX indicates XXX is the file name on server */
		file_put_contents($_GET['mpd'], $mpdout, LOCK_EX);
	}
	if ($DUMPMPD) {
		file_put_contents("dumpedmpd.mpd", $mpdout, LOCK_EX);
	}

	if($REDIRECTTOFILE)
		ob_end_flush();
	

	exit;

case 'seg':
	/* incoming request for a segment */
	if (!isset($_GET['orig_url'])) {
		die("orig_url not specified in media segment request (abort)");
	}
	$targetname = $_GET['orig_url'];
	if ($DEBUG) {
		echo "*** PROCESSING SEGMENT REQ>".$targetname."<\n";
	}
	process_segment_request($targetname);	// handles absolute URLs

	if($REDIRECTTOFILE)
		ob_end_flush();
	
	exit;

default:
	die("request for unkown type of item; type=XX unknown XX");
}

/* NOTREACHED */

/*
 * process incoming request for an MPD
 * returns a dynamically-generated MPD in live profile format
 */

function process_mpd($mpdurl)
{
	global $DEBUG, $request_uri;

	$path_parts = pathinfo($mpdurl);
	$mpdfilename = $path_parts['filename'];		// determine name of actual MPD file

	if ($DEBUG) {
		echo "****** PROCESS_MPD mpdurl: " . $mpdurl . "\n";
		echo "****** PROCESS_MPD mpdfilename: " . $mpdfilename . "\n";
		echo "****** PROCESS_MPD request_uri: " . $request_uri . "\n";
	}

	/* load/parse actual MPD (an XML file) */
	libxml_use_internal_errors(true);

	if($_GET['origmpd'])	//If location of MPD is provided in the request

		$MPD = simplexml_load_file($_GET['origmpd']);
	else

		$MPD = simplexml_load_file($mpdfilename);
	if (!$MPD) {
		die("Failed loading XML file");
	}

	/* re-write as a live one */
	od2live($request_uri, $MPD);

        $dom_sxe = dom_import_simplexml($MPD);
        if (!$dom_sxe) {
            echo 'Error while converting XML';
            exit;
        }
            $dom = new DOMDocument('1.0');
            $dom_sxe = $dom->importNode($dom_sxe, true);
            $dom_sxe = $dom->appendChild($dom_sxe);

            foreach ($dom->documentElement->childNodes as $node) {
            //echo $node->nodeName; // body
            if($node->nodeName === 'Location')
                $locationNode = $node;
            if($node->nodeName === 'BaseURL')
                $baseURLNode = $node;    
            if($node->nodeName === 'Period')
                $periodNode = $node;    
        }
        $periodNode->setAttribute("start","PT0S");
        $periodNode->setAttribute("id","1");
        if($baseURLNode)
            $periodNode->parentNode->insertBefore($baseURLNode,$periodNode);
        
        if($locationNode)
            $periodNode->parentNode->insertBefore($locationNode,$periodNode);

		if($_GET['UTCTiming'])
		{ 
			$own_link = "http://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
			$UTCTiming=$dom->createElement('UTCTiming');
			
			switch ($_GET['UTCTiming'])
			{
				case 'ntp':
					$UTCTiming->setAttribute("schemeIdUri","urn:mpeg:dash:utc:ntp:2014");
					$UTCTiming->setAttribute("value","0.pool.ntp.org");
				break;

				case 'ntpc':
					$UTCTiming->setAttribute("schemeIdUri","urn:mpeg:dash:utc:sntp:2014");
					$UTCTiming->setAttribute("value","ntp.amnic.net");
				break;

				case 'httph':
					$UTCTiming->setAttribute("schemeIdUri","urn:mpeg:dash:utc:http-head:2014");
					$UTCTiming->setAttribute("value",$own_link);
				break;

				case 'httpg':
					$UTCTiming->setAttribute("schemeIdUri","urn:mpeg:dash:utc:http-xsdate:2014");
					$UTCTiming->setAttribute("value",$own_link);
				break;

				case 'httpi':
					$UTCTiming->setAttribute("schemeIdUri","uurn:mpeg:dash:utc:http-iso:2014");
					$UTCTiming->setAttribute("value",$own_link);
				break;

				case 'httpn':
					$UTCTiming->setAttribute("schemeIdUri","uurn:mpeg:dash:utc:http-ntp:2014");
					$UTCTiming->setAttribute("value",$own_link);
				break;

				case 'utcd':
					$UTCTiming->setAttribute("schemeIdUri","uurn:mpeg:dash:utc:direct:2014");
					$UTCTiming->setAttribute("value",$periodNode->parentNode->getAttribute('availabilityStartTime'));
				break;
				
				default:
					die("request for unkown type of UTCTiming");
			}
			
			$periodNode->parentNode->appendChild($UTCTiming);			
		}
        
        return ($dom->saveXML());
}

/*
 * process incoming request for a segment
 * checks to see if request is in valid time window
 * if so, returns the actual segment requested
 * if not, returns HTTP error (404)
 * NB: this function must deal with byte range style requests
 */

function process_segment_request($url)
{
	global $DEBUG, $SEGDEBUG;

	if ($SEGDEBUG) {
		echo "***** PROCESS SEGMENT REQUEST for >" . $url . "\n";
	}

	$nowts = time();
	/* see if tsbd= was provided */
	if (isset($_GET['tsbd'])) {
		$tsbdstr = $_GET['tsbd'];
		$left_window = $nowts - intval($tsbdstr);
	} else {
		/* no tsbd means infinity */
		$left_window = 0;
	}

	/* get when MPD last fetched -- mandatory */
	if (isset($_GET['pt'])) {
		$fetch_time = $_GET['pt'];
	} else {
		die("no MPD Publish Time when processing segment");
	}

	/* get when MPD last updated and figure out right window edge */
	if (isset($_GET['mup'])) {
		$mup = $_GET['mup'];
		$right_window = min($mup+$fetch_time, $nowts);
	} else {
		$right_window = $nowts;
		$mup = $nowts;
	}

	/* get info about segment: duration, start number, start time */
	if (isset($_GET['dur']))
		$duration = $_GET['dur'];
        
	if (isset($_GET['tscale']))
		$timescale = $_GET['tscale'];
        else
            $timescale = 1;
        
	if (isset($_GET['stnum']))
		$startnum = $_GET['stnum'];
	if (isset($_GET['stime']))
		$starttime = $_GET['stime'];

	/*
	 * figure out the start time of the requested segment
	 */

        /* check for sentinels (^s), which surround number (number case only) */
        $url = urldecode($url);
        if (preg_match('/\^(.*)\^/', $url, $matches) > 0) {
                /* number-based : see 5.3.9.5.4 of spec */
                $segno = intval($matches[1]);
		$mpd_start_time = ($segno - $startnum - 1) * round ($duration/$timescale,PHP_ROUND_HALF_UP);	/* can be negative */
                $start = $starttime + $mpd_start_time + round ($duration/$timescale,PHP_ROUND_HALF_UP);	/* doesn't yet include AST adjustment */
               
                /* remove sentinels */
                $url = preg_replace('/\^(.*)\^/', strval($segno), $url);
        }   

        /* check for sentinels (=s), which surround time stamps (time case only) */
        if (preg_match('/\=(.*)\=/', $url, $matches) > 0) {
                /* time based */
                $start = $matches[1];
                /* remove sentinels */
                $url = preg_replace('/\=(.*)\=/', $start, $url);
        }   

	/* we may not have a segment start (e.g., if no template) */
	if (!isset($start)) {
		if (!isset($starttime)) {
			die("couldn't determine a start time");
		}
		$start = $starttime;
	}

	/* availability start time -- mandatory */
	if (isset($_GET['avail_start'])) {
		$avail = $_GET['avail_start'];
	} else {
		die("no availability start time in segment request");
	}
	/* adjust relative start to absolute start */
	$start += intval($avail);
        $left_window_edge = $start;
        
        if (isset($_GET['tsbd'])) {
		$tsbdstr = $_GET['tsbd'];
		$right_window_edge = $left_window_edge + intval($tsbdstr);
	} else {
		/* no tsbd means infinity */
		$right_window_edge = $nowts;    //It will be available now, whatever it is
	}

	//$in_window = (($left_window <= $start) && ($start <= $right_window));
        $in_window = (($left_window_edge <= $nowts) && ($nowts <= $right_window_edge));
        
                file_put_contents ( "logFile2.txt" , "***** DESIRED_SEGMENT: " . $url . "\n" ,FILE_APPEND );
		file_put_contents ( "logFile2.txt" , "***** NOW: " . $nowts . "\n" ,FILE_APPEND ); 
		file_put_contents ( "logFile2.txt" , "***** WINDOW: [" . $left_window_edge . "," . $right_window_edge . "]\n" ,FILE_APPEND ); 
		file_put_contents ( "logFile2.txt" , "***** START: "  . $start . "\n" ,FILE_APPEND ); 
		file_put_contents ( "logFile2.txt" , "***** MUP: "  . $mup . "\n" ,FILE_APPEND ); 
		file_put_contents ( "logFile2.txt" , "***** FETCH_TIME: "  . $fetch_time . "\n" ,FILE_APPEND ); 
		file_put_contents ( "logFile2.txt" , "***** AST: "  . $avail . "\n" ,FILE_APPEND ); 
		if (isset($tsbdstr)) {
			file_put_contents ( "logFile2.txt" , "***** TSBD: "  . $tsbdstr . "\n" ,FILE_APPEND ); 
		}
		if ($in_window) {
			file_put_contents ( "logFile2.txt" , "***** SEGMENT IS IN WINDOW +++++\n" ,FILE_APPEND ); 
		} else {
			file_put_contents ( "logFile2.txt" , "***** SEGMENT IS NOT IN WINDOW ----\n" ,FILE_APPEND ); 
		}

	if ($SEGDEBUG) {
		echo "***** DESIRED_SEGMENT: " . $url . "\n";
		echo "***** NOW: " . $nowts . "\n";
		echo "***** WINDOW: [" . $left_window . "," . $right_window . "]\n";
		echo "***** START: "  . $start . "\n";
		echo "***** MUP: "  . $mup . "\n";
		echo "***** FETCH_TIME: "  . $fetch_time . "\n";
		echo "***** AST: "  . $avail . "\n";
		if (isset($tsbdstr)) {
			echo "***** TSBD: "  . $tsbdstr . "\n";
		}
		if ($in_window) {
			echo "***** SEGMENT IS IN WINDOW +++++\n";
		} else {
			echo "***** SEGMENT IS NOT IN WINDOW ----\n";
		}
	}

//header("Content-Location: " . $url);	// helpful for debugging packet traces

	/* is requested segment in availability window? easy check */
	if ($in_window) {

		//file_put_contents ( 'mynewlog.txt' ,  'Location: ' . $url);

		//Just redirect and exit (following code is not used that assumed content on local host)
		header('Location: ' . $url);
		exit;
		/* segment timing is ok */
		$purl = parse_url($url);
		$fname = $_SERVER['DOCUMENT_ROOT'] . $purl['path'];
		if ($SEGDEBUG) {
			echo "*** would have transferred contents of file " . $fname . "\n";
			exit;
		}

		rangeDownload($fname);
		exit;
	}

	/* note that if right window is < left window, perhaps MPD is out of date */
	header("Warning: seg not avail start: " . $start . " lwind: " . $left_window . " rwind: " . $right_window);
	header("HTTP/1.0 404 Not Found");  
	exit;
}

/*
 * re-write a (static) MPD to be a dynamic/live one
 */

function od2live($fetch_url, $MPD)
{
	global $DEBUG, $tz, $mupival;
	global $ast;
	global $tsbd;


	/* MPD@type attribute */
	$t = strtolower($MPD['type']);
	switch ($t) {
	case 'ondemand':
		$old = True;
		$acceptable = True;
		break;
	case 'live':
		$old = True;
		$acceptable = False;
		break;
	case 'static':
		$old = False;
		$acceptable = True;
		break;
	case 'dynamic':
		$old = False;
		$acceptable = False;
		break;
	default:
		echo "Unknown MPD type " . $t;
		exit;
	}
	$MPD['type'] = 'dynamic';	// force dynamic type MPD

	/* MPD@profiles */
	$MPD['profiles'] = Profiles;

	/* MPD@availabilityStartTime */
	if (isset($_GET['avail_start'])) {
		/* if present, uses the unix time */
		$ast = DateTime::createFromFormat("U", $_GET['avail_start'], $tz);
		$ast->setTimestamp(intval($_GET['avail_start']));
	} else {
		$ast = new DateTime(DEFAULT_AST, $tz);
	}
	$MPD['availabilityStartTime'] = $ast->format(DATE_TIME_FORMAT);

	/* MPD@availabilityEndTime (optional) */
	if (DEFAULT_AET != 'none') {
		$et = clone $ast;
		$ival = new DateInterval(DEFAULT_AET);
		$et->add($ival);
		$MPD['availabilityEndTime'] = $et->format(DATE_TIME_FORMAT);
	}

	/*
	 * originally, unset mediaPresentationDuration here
	 * however, leaving it in place allows us to indicate end of the live
	 * Presentation.  An alternative approach may be to set @availabilityEndTime as
	 * the sum of @availabilityStartTime + @mediaPresentationDuration
	 * Unclear (to me) what the spec prefers
	 */

	/* MPD@minimumUpdatePeriod (optional but not unusual) */
	/* allow override of this using mup= query string */
	if (isset($_GET['mup']) && $_GET['mup'] != "0") {
		/* if presesent, will be in seconds */
		$istr = 'PT' . $_GET['mup'] . 'S';
		$mupival = new DateInterval($istr);
		$MPD['minimumUpdatePeriod'] = $istr;
	} else if ( DEFAULT_MUP != 'none' ) {
		$mupival = new DateInterval(DEFAULT_MUP);
		$MPD['minimumUpdatePeriod'] = $mupival->format(DEFAULT_MUP);
	}

	/* MPD@suggestedPresentationDelay (optional but not unusual) */
	if (isset($_GET['spd'])) {
		/* if presesent, will be in seconds */
		$istr = 'PT' . $_GET['spd'] . 'S';
		$spdival = new DateInterval($istr);
		$MPD['DEFAULT_SPD'] = $istr;
	} else if ( DEFAULT_SPD != 'none' ) {
		$ival = new DateInterval(DEFAULT_SPD);
		$MPD['suggestedPresentationDelay'] = $ival->format(DEFAULT_SPD);
	}

	/* MPD@timeShiftBufferDepth (optional) */
	$left_window = new DateTime("now", $tz);
	if (isset($_GET['tsbd']) && $_GET['tsbd'] != "0") {
		/* if presesent, will be in seconds */
		$istr = 'PT' . $_GET['tsbd'] . 'S';
		$tsbd = new DateInterval($istr);
		$MPD['timeShiftBufferDepth'] = $istr;
		$left_window->sub($tsbd);
	} else if ( DEFAULT_TSBD != 'infinite' ) {
		/* finite default from this file */
		$tsbd = new DateInterval(DEFAULT_TSBD);
		$MPD['timeShiftBufferDepth'] = $tsbd->format(DEFAULT_TSBD); 
		$left_window->sub($tsbd);
	} else {
		/* infinite and no override */
		$left_window->modify(UNIX_ZERO_TS);	/* UNIX epoch */
	}

	/* MPD@publishTime is required for 'dynamic' MPDs according to PDAM 1 (Aug 2012) */
	$pt = new DateTime("now", $tz);
	if (isset($_GET['pt'])) {
		/* if present, uses the unix time */
		$pt->setTimestamp(intval($_GET['pt']));
	}
	//$pt->setTimestamp(ntpts($pt->format("U")));
	
	$MPD['publishTime'] = $pt->format(DATE_TIME_FORMAT);


	/* now the harder part... structure is MPD|Period|Adaptation Set| ContentComponent or Rep1 ... Repn */

	/* loop through top-level of MPD */
	$found_baseurl = 0;		// found a BaseURL element at top level
	$found_location = 0;		// found a Location element at top level

	if ($MPD->count() == 0) {
		var_dump($MPD);
		die("MPD has no contents! aborting");
	}

	foreach ($MPD->children() as $child) {
		$elname = $child->getName();
		if ($DEBUG == 1) {
			echo "****** PROCESSING TOP-LEVEL-MPD ITEM " . $elname . "\n";
		}
		switch ($elname) {
		case 'Period':
			process_period($child, $pt, $fetch_url);
			break;
		case 'Location':
			$found_location = 1;
			break;
		case 'BaseURL':
			process_seginfo($child, $pt, $fetch_url);
			$found_baseurl = 1;
			break;
		default:
			if ($DEBUG) {
				echo "****** DIDNT NEED TO PROCESS MPD@element " . $elname;
			}
			break;
		}
	}

	/* if there is no BaseURL element, create one */
	if ($found_baseurl == 0) {
		$str = request_uri_long();
		$str .= "?type=seg";
		$str .= "&pt=" . $pt->format("U");
		/* if no AST provided, use MPD publish time */
		$str .= "&avail_start=" . ((isset($ast)) ? $ast->format("U") : $pt->format("U"));

		if (isset($tsbd)) {
			$str .= "&tsbd=" . $tsbd->format("%S");
		}
		if (isset($mupival)) {
			$str .= "&mup=" . $mupival->format("%S");
		}
		$str .=  "&orig_url=";	// must come last

		if($_GET['origmpd'])
		{
                        $path_parts = pathinfo($_GET['origmpd']);

			$str .= $path_parts['dirname'] . "/";	//Path to orig MPD
		}
		$MPD->BaseURL = $str;
	}

	/* if there is no Location element, create one and include the AST */
	if ($found_location == 0) {
		$str = request_uri_long();
		$str .= "?type=mpd";
		$str .= "&avail_start=" . $ast->format("U");
		$str .= "&pt=" . $pt->format("U");
		
		if(isset($tsbd))
		$str .= "&tsbd=" . $tsbd->format("%S");
		
		if(isset($mupival))
		$str .= "&mup=" . $mupival->format("%S");
		

		if($_GET['origmpd'])
			$str .= "&origmpd=" . $_GET['origmpd'];
		$MPD->Location = $str;
	} else {
		$MPD->Location .= "&pt=" . $pt->format("U") . "&avail_start=" . $ast->format("U");
		if($_GET['origmpd'])
			$MPD->Location .= "&origmpd=" . $_GET['origmpd'];
	}
	$MPD->Location = mpdhack($MPD->Location);
	return;
} /* od2live */

/*
 * process a Period
 * if we happen to have Period.start and Period.duration, easy
 * otherwise, have to dig into the segment information in the
 * adaptation set(s)
 */

function process_period($period, $fetch_time, $url)
{
	global $DEBUG;

	/* we're lucky if the Period already has start and duration */
	if (isset($child['start']) && isset($child['duration'])) {
		die("don't yet support defined Period start/duration times");
	} else {
		/* not lucky, go deeper into Period */
		foreach ($period->children() as $child) {
			$elname = $child->getName();
			switch ($elname) {
			case 'AdaptationSet':
		
		if ($DEBUG == 1)
		
			echo "****** PROCESSING Adaptation set \n";
				process_adaptation_set($child, $fetch_time, $url);
				break;
			case 'BaseURL':
			case 'SegmentBase':
			case 'SegmentTemplate':
			case 'SegmentList':
				process_seginfo($child, $fetch_time, $url);
				break;
			case 'Subset':
				die("don't know how to handle Subset in Period");
			}

		}
	}
}

/*
 * Adaptation Sets contain Representations
 * and maybe Segment Information
 * Other info we don't care about
 * (like ContentComponent)
 */
function process_adaptation_set($aset, $fetch_time, $url)
{
	global $DEBUG;
	foreach ($aset->children() as $child) {
		$elname = $child->getName();

		if ($DEBUG == 1)

			echo "****** PROCESSING " . $elname . "\n\n";
		switch ($elname) {
		case 'BaseURL':
		case 'SegmentBase':
		case 'SegmentTemplate':
		case 'SegmentList':
			process_seginfo($child, $fetch_time, $url);
			break;
		case 'Representation':
			process_representation($child, $fetch_time, $url);
			break;
		}
	}
}

/*
 * Representation has the segment information we care about
 */

function process_representation($rep, $fetch_time, $url)
{
	global $DEBUG;

	$addressingCount = 0;
	$baseURLFound = False;
	foreach ($rep->children() as $child) {
		$elname = $child->getName();
		switch ($elname) {
		case 'BaseURL':

		$baseURLFound = True;
		case 'SegmentBase':
		case 'SegmentTemplate':
		case 'SegmentList':

			$addressingCount++;
			process_seginfo($child, $fetch_time, $url);
			break;
		case 'SubRepresentation':
			die("don't know how to handle SubRepresentation in Representation");

		}
	}

	if($addressingCount == 1 && $baseURLFound == True )	//Special case of only using Base URL, typically means a single segment
	{
	
	foreach ($rep->children() as $child) {

	
		$elname = $child->getName();

	
		switch ($elname) {

	
		case 'BaseURL':

			$child[] .= "&stime=0";	//todo: For now, no offsets found

	
			break;
	
	
		}

	
	}
	}
}

/*
 * Segment Information (location and availability)
 * (see section 5.3.9 of standard)
 * can be conveyed in 3 ways:
 *   BaseURL, SegmentBase, SegmentTemplate and/or SegmentList
 * (these can also appear at the Period and AdaptationSet level)
 * SegmentTemplate can use time or segment index
 */

function process_seginfo($segment, $fetch_time, $url)
{
	global $DEBUG;
	global $tsbd;
	global $ast;
	
	global $mupival;

	$elname = $segment->getName();
	switch ($elname) {
	case 'BaseURL':
		/* if it starts with http:, then prepend our URL; if its ./, put path of MPD*/

		if($segment == './')
		{
			if($_GET['origmpd'])
			{
			
	$path_parts = pathinfo($_GET['origmpd']);
				$segment[] = $path_parts['dirname'] . "/";	//Path to orig MPD
			}
			else
			{
			
	$path_parts = pathinfo(request_uri_long());
				$segment[] = $path_parts['dirname'] . "/";	//Path to PHP
			}
		}
		$n = strpos($segment, 'http');
		if ($n !== False) {	/* note use of '!==' in php */
			/* modify the 'real' BaseURL to point to us */
			$Base = $segment;
			$str = $url .
				"?type=seg" .
				"&pt=" . $fetch_time->format("U") .
				"&avail_start=" . $ast->format("U");


			if (isset($tsbd)) {
				$str .= "&tsbd=" . $tsbd->format("%S");
			}

			if (isset($mupival)) {
				$str .= "&mup=" . $mupival->format("%S");
			}

			$str .= '&orig_url=' . $Base;
			$segment[] = $str;
		}
		break;
	case 'SegmentTemplate':
		$startoffset = 0;
		if ($segment->count() != 0) {
			/* we don't do SegmentTimeline just now */
			die("unable to process SegmentTemplate element with child");
			exit;
		}
		if (isset($segment['duration']))
			$duration = $segment['duration']; /* scale to secs */
		if (isset($segment['timescale']))
			$timescale = $segment['timescale'];
                if (isset($segment['startNumber']))
			$startnumber = $segment['startNumber'];
		if (isset($segment['presentationTimeOffset']))
			$startoffset = $segment['presentationTimeOffset'];

		$str = "&dur=" . $duration;
                
		if (isset($segment['timescale']))
                    $str .= "&tscale=" . $timescale;
                
                $str .= "&stnum=" . $startnumber . "&stime=" . $startoffset;

		/*
		 * these are all URIs that need to be 'sentinelized'
		 * the issue is that when $Time$ or $Number$ are replaced, the sentinals
		 * disappear.  So, something like "seg$Number$1" becomes "seg11" for example...
		 * Is it segment 1 or 11?  So, we force the issue by adding our own sentinels.
		 */
		if (isset($segment['media'])) {
			$segment['media'] .= $str;
			$segment['media'] = preg_replace(NumPattern, NumReplace, $segment['media']);
			$segment['media'] = preg_replace(TimePattern, TimeReplace, $segment['media']);
		}
		if (isset($segment['initialization'])) {
			$segment['initialization'] .= $str;
			$segment['initialization'] = preg_replace(NumPattern, NumReplace, $segment['initialization']);
			$segment['initialization'] = preg_replace(TimePattern, TimeReplace, $segment['initialization']);
		}
		/* for the folks over the pond that spell initialization with an 's' */
		if (isset($segment['initialisation'])) {
			$segment['initialization'] = $segment['initialisation'] . $str;
			/* not that these aren't allowed to have $Number$ or $Time$ (Table 15 in spec) */
		}
		if (isset($segment['index'])) {
			$segment['index'] .= $str;
			$segment['index'] = preg_replace(NumPattern, NumReplace, $segment['index']);
		}
		break;
	case 'SegmentList':
		if (isset($segment['duration'])) {
			/* nothing to do with this for now */
		}
		foreach ($segment->children() as $child) {
			$elname = $child->getName();
			switch ($elname) {
			case 'SegmentURL':
				$str = "&stime=" .  $MPD['availabilityStartTime'];
				$child['media'] .= $str;
				break;
			}
		}
	}
}

/*
 * this is based somewhat on:
 *   http://www.phpfreaks.com/forums/index.php?topic=198274.0
 *   25-May-2012
 *
 * copy the file from the file system to the output
 * obey range requests if that is what we received
 */

function rangeDownload($file)
{
	if (!is_file($file)) {
		header("Warning: file " . $file . " isn't a file");
		return;
	}

	header("Content-Type: " . mime_content_type($file));
	$filelength = filesize($file);

	// see if its a range request (usually will be for DASH)
	if (isset($_SERVER['HTTP_RANGE'])) {
		if (!preg_match('/^bytes=\d*-\d*(,\d*-\d*)*$/', $_SERVER['HTTP_RANGE'])) {
			// yes, but a bad one
			header('HTTP/1.1 416 Requested Range Not Satisfiable(1)');
			header('Content-Range: bytes */' . $filelength); // Required in 416.
			return;
		}

		// check for multiple ranges
		$ranges = explode(',', substr($_SERVER['HTTP_RANGE'], 6));
		foreach ($ranges as $range) {
			$parts = explode('-', $range);
			$start = $parts[0]; // If this is empty, this should be 0.
			$end = $parts[1]; // If this is empty or greater than than filelength - 1, this should be filelength - 1.

			if ($end === "")
				$end = strval($filelength-1);

			if (intval($start) > intval($end)) {
			    header('HTTP/1.1 416 Requested Range Not Satisfiable(2)');
			    header('Content-Range: bytes */' . $filelength); // Required in 416.
			    return;
			}

		}

		// for now only really handles the last of the subranges
		$fp = @fopen($file, 'rb');
		
		// Now that we've gotten so far without errors we send the accept range header
		/* At the moment we only support single ranges.
		 * Multiple ranges requires some more work to ensure it works correctly
		 * and comply with the spesifications: http://www.w3.org/Protocols/rfc2616/rfc2616-sec19.html#sec19.2
		 *
		 * Multirange support annouces itself with:
		 * header('Accept-Ranges: bytes');
		 *
		 * Multirange content must be sent with multipart/byteranges mediatype,
		 * (mediatype = mimetype)
		 * as well as a boundry header to indicate the various chunks of data.
		 */

		header('Accept-Ranges: bytes');

		// header("Accept-Ranges: 0-" . $filelength);
		// multipart/byteranges
		// http://www.w3.org/Protocols/rfc2616/rfc2616-sec19.html#sec19.2

		$length = $end - $start + 1; // Calculate new content length
		fseek($fp, $start);
		header('HTTP/1.1 206 Partial Content');

		// Notify the client the byte range we'll be outputting
		header("Content-Range: bytes $start-$end/$filelength");
		header("Content-Length: $length");

		// header("Connection: close"); -- have seen this but shouldn't be necessary

		// Start buffered download
		$buffer = 1024 * 8;
		while(!feof($fp) && ($p = ftell($fp)) <= $end)
		{
			if ($p + $buffer > $end)
			{
				// In case we're only outputting a chunk, make sure we don't
				// read past the length
				$buffer = $end - $p + 1;
			}
			set_time_limit(0); // Reset time limit for big files
			echo fread($fp, $buffer);
			flush(); // Free up memory. Otherwise large files will trigger PHP's memory limit.
		}

		fclose($fp);
	} else {
		// no byte range
		header("Content-Length :" . $filelength);
		readfile($file);
	}
}

?>

