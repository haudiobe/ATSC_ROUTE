<!DOCTYPE html>
<!--
Main script to start playback.
Provide the correct dash.js URL as "playerURL" below
Start a channel before
-->

<html>
<head>
<meta charset="utf-8" />
<meta http-equiv="X-UA-Compatible" content="IE=Edge,chrome=1" />
<title>ROUTE Demo</title>
<link href='http://fonts.googleapis.com/css?family=Open+Sans' rel='stylesheet' type='text/css'>
<style>
::selection {
  color: #fff;
  background: pink;
}
html, body {
  overflow: hidden;
  height: 100%;
}
body {
  margin: 0;
}
body {
  color: #222;
  font-family: 'Open Sans', arial, sans-serif;
  display: -webkit-flex;
  -webkit-align-items: center;
  -webkit-justify-content: center;
  -webkit-flex-direction: column;
  display: -ms-flex;
  -ms-align-items: center;
  -ms-justify-content: center;
  -ms-flex-direction: column;
  display: -moz-flex;
  -moz-align-items: center;
  -moz-justify-content: center;
  -moz-flex-direction: column;
  display: -o-flex;
  -o-align-items: center;
  -o-justify-content: center;
  -o-flex-direction: column;
  display: flex;
  align-items: center;
  justify-content: center;
  flex-direction: column;
}
details {
  position: absolute;
  top: 1em;
  left: 1em;
  margin: 1em 0;
  cursor: pointer;
  padding: 10px;
  background: #fff;
  border: 1px solid rgba(0,0,0,0.3);
  border-radius: 5px;
  max-width: 600px;
  font-size: 10pt;
  z-index: 100;
}
details > div {
  margin: 10px 0;
}
details blockquote {
  font-style: italic;
}
pre:not(#log) {
  background: #eee;
  border-radius: 5px;
  padding: 3px 17px 20px 17px;
  border: 1px solid #ccc;
  color: navy;
}
#log {
  margin: 0 1em;
}
code {
  font-weight: bold;
}
section {
  display: -webkit-flex;
  display: flex;
}
#carousel-wrapper {
    padding-bottom: 10px;
    position: relative;
}
#carousel, #thumbs {
    overflow: hidden;
}
#carousel-wrapper .caroufredsel_wrapper {
    border-radius: 10px;
    box-shadow: 0 0 5px #899;
}

#carousel span, #carousel img,
#thumbs a, #thumbs img  {
    display: block;
    float: left;
}
#carousel span, #carousel a,
#thumbs span, #thumbs a {
    position: relative;
}
#carousel img,
#thumbs img {
    border: none;
    width: 100%;
    height: 100%;
    position: absolute;
    top: 0;
    left: 0;
}
#carousel img.glare,
#thumbs img.glare {
    width: 102%;
    height: auto;
}
#thumbs-wrapper {
    position: relative;
}
#thumbs a {
    border: 2px solid #899;
    width: 120px;
    height: 60px;
    margin: 5px 10px;
    overflow: hidden;
    border-radius: 10px;

    -webkit-transition: border-color .5s;
    -moz-transition: border-color .5s;
    -ms-transition: border-color .5s;
    transition: border-color .5s;
}
#thumbs a:hover, #thumbs a.selected {
    border-color: #566;
}

#image{
    position: absolute;
    top:10px;
    right:100px;
    width:35px;
    height:35px;
}
#Settingslink{
    position: absolute;
    top:15px;
    right:30px;  
    color:black; text-decoration:none
}
</style>
</head>

    <body id="demopage">
	<h3>ROUTE Demo</h3>
	
<div style="background-image: url(thumbs/background.png); height: 540px; width: 960px;">
  <video controls width="960" height="540" id="videoPlayer" onplay="playEvent()"></video>
</div>
<section>
  <pre id="log"></pre>
</section>
  <div id="streamWrapper">
    <h3>Choose a Channel</h3>
    <div style="height: 150px; display:inline; vertical-align: middle;">
      <div id="thumbs" style="display:inline-flex;">
        <a href="#" title="Channel 1" onclick="start(1);return false;"><img src="thumbs/thumb1.png" /></a>
        <a href="#" title="Channel 2" onclick="start(2);return false;"><img src="thumbs/thumb2.png" /></a>
      </div>
    </div>
  </div>
 
        <img src="SettingsIcon.png" id="image">
        <a id="Settingslink" href="../../../ReceiverUI/index.php" target="_blank">Settings</a>
    </body>
</html>

<?php 
;
?>

<script src="jquery-1.11.1.min.js"></script>
    <!-- http://angularjs.org/ -->
    <script src="dash.js/samples/dash-if-reference-player/app/lib/angular/angular.min.js"></script>
    <script src="dash.js/samples/dash-if-reference-player/app/lib/angular/angular-resource.min.js"></script>

    <!-- http://getbootstrap.com/ -->
    <script src="dash.js/samples/dash-if-reference-player/app/lib/bootstrap/js/bootstrap.min.js"></script>

    <!-- https://github.com/creynders/dijon-framework -->
    <script src="dash.js/src/lib/dijon.js"></script>
    
    <!-- https://github.com/madebyhiro/codem-isoboxer -->
    <script src="dash.js/src/lib/iso_boxer.min.js"></script>

    <!-- http://bannister.us/weblog/2007/06/09/simple-base64-encodedecode-javascript/ -->
    <script src="dash.js/src/lib/base64.js"></script>

    <!-- Misc Libs -->
    <script src="dash.js/src/lib/xml2json.js"></script>
    <script src="dash.js/src/lib/objectiron.js"></script>
    <script src="dash.js/src/lib/long.js"></script>
    <script src="dash.js/src/lib/Math.js"></script>

    <!-- http://www.flotcharts.org/ -->
    <script src="dash.js/samples/dash-if-reference-player/app/lib/flot/jquery.flot.js"></script>

    <!-- https://github.com/eu81273/angular.treeview -->
    <script src="dash.js/samples/dash-if-reference-player/app/lib/angular.treeview/angular.treeview.min.js"></script>

   <!-- Player -->
    <script src="dash.js/src/streaming/MediaPlayer.js"></script>
    <script src="dash.js/src/streaming/Context.js"></script>
    <script src="dash.js/src/streaming/ErrorHandler.js"></script>
    <script src="dash.js/src/streaming/utils/Capabilities.js"></script>
    <script src="dash.js/src/streaming/utils/DOMStorage.js"></script>
    <script src="dash.js/src/streaming/utils/EventBus.js"></script>
    <script src="dash.js/src/streaming/utils/Debug.js"></script>
    <script src="dash.js/src/streaming/utils/CustomTimeRanges.js"></script>
    <script src="dash.js/src/streaming/utils/VirtualBuffer.js"></script>
    <script src="dash.js/src/streaming/utils/BoxParser.js"></script>
    <script src="dash.js/src/streaming/utils/IsoFile.js"></script>
    <script src="dash.js/src/streaming/extensions/RequestModifierExtensions.js"></script>
    <script src="dash.js/src/streaming/models/VideoModel.js"></script>
    <script src="dash.js/src/streaming/vo/FragmentRequest.js"></script>
    <script src="dash.js/src/streaming/vo/TrackInfo.js"></script>
    <script src="dash.js/src/streaming/vo/TextTrackInfo.js"></script>
    <script src="dash.js/src/streaming/vo/MediaInfo.js"></script>
    <script src="dash.js/src/streaming/vo/StreamInfo.js"></script>
    <script src="dash.js/src/streaming/vo/TextTrackInfo.js"></script>
    <script src="dash.js/src/streaming/vo/ManifestInfo.js"></script>
    <script src="dash.js/src/streaming/vo/Event.js"></script>
    <script src="dash.js/src/streaming/vo/Error.js"></script>
    <script src="dash.js/src/streaming/vo/BitrateInfo.js"></script>
    <script src="dash.js/src/streaming/vo/DataChunk.js"></script>
    <script src="dash.js/src/streaming/vo/IsoBox.js"></script>
    <script src="dash.js/src/streaming/ManifestLoader.js"></script>
    <script src="dash.js/src/streaming/ManifestUpdater.js"></script>
    <script src="dash.js/src/streaming/models/ManifestModel.js"></script>
    <script src="dash.js/src/streaming/extensions/MediaSourceExtensions.js"></script>
    <script src="dash.js/src/streaming/extensions/SourceBufferExtensions.js"></script>
    <script src="dash.js/src/streaming/extensions/VideoModelExtensions.js"></script>
    <script src="dash.js/src/streaming/controllers/PlaybackController.js"></script>
    <script src="dash.js/src/streaming/controllers/FragmentController.js"></script>
    <script src="dash.js/src/streaming/controllers/AbrController.js"></script>
    <script src="dash.js/src/streaming/FragmentLoader.js"></script>
    <script src="dash.js/src/streaming/models/FragmentModel.js"></script>
    <script src="dash.js/src/streaming/controllers/StreamController.js"></script>
    <script src="dash.js/src/streaming/controllers/MediaController.js"></script>
    <script src="dash.js/src/streaming/StreamProcessor.js"></script>
    <script src="dash.js/src/streaming/controllers/ScheduleController.js"></script>
    <script src="dash.js/src/streaming/TimeSyncController.js"></script>
    <script src="dash.js/src/streaming/Stream.js"></script>
    <script src="dash.js/src/streaming/controllers/BufferController.js"></script>
    <script src="dash.js/src/streaming/LiveEdgeFinder.js"></script>
    <script src="dash.js/src/streaming/Notifier.js"></script>
    <script src="dash.js/src/streaming/controllers/EventController.js"></script>
    <script src="dash.js/src/streaming/models/URIQueryAndFragmentModel.js"></script>
    <script src="dash.js/src/streaming/vo/URIFragmentData.js"></script>
    <script src="dash.js/src/streaming/controllers/XlinkController.js"></script>
    <script src="dash.js/src/streaming/XlinkLoader.js"></script>

    <!--Protection-->
    <script src="dash.js/src/streaming/models/ProtectionModel.js"></script>
    <script src="dash.js/src/streaming/models/ProtectionModel_01b.js"></script>
    <script src="dash.js/src/streaming/models/ProtectionModel_3Feb2014.js"></script>
    <script src="dash.js/src/streaming/models/ProtectionModel_21Jan2015.js"></script>
    <script src="dash.js/src/streaming/vo/protection/ClearKeyKeySet.js"></script>
    <script src="dash.js/src/streaming/vo/protection/KeyPair.js"></script>
    <script src="dash.js/src/streaming/vo/protection/SessionToken.js"></script>

    <script src="dash.js/src/streaming/controllers/ProtectionController.js"></script>
    <script src="dash.js/src/streaming/extensions/ProtectionExtensions.js"></script>
    <script src="dash.js/src/streaming/protection/CommonEncryption.js"></script>

    <script src="dash.js/src/streaming/protection/drm/KeySystem.js"></script>
    <script src="dash.js/src/streaming/protection/drm/KeySystem_Access.js"></script>
    <script src="dash.js/src/streaming/protection/drm/KeySystem_ClearKey.js"></script>
    <script src="dash.js/src/streaming/protection/drm/KeySystem_PlayReady.js"></script>
    <script src="dash.js/src/streaming/protection/drm/KeySystem_Widevine.js"></script>
    <script src="dash.js/src/streaming/protection/drm/KeySystem.js"></script>

    <script src="dash.js/src/streaming/protection/servers/ClearKey.js"></script>
    <script src="dash.js/src/streaming/protection/servers/PlayReady.js"></script>
    <script src="dash.js/src/streaming/protection/servers/Widevine.js"></script>
    <script src="dash.js/src/streaming/protection/servers/DRMToday.js"></script>

    <script src="dash.js/src/streaming/vo/protection/KeyError.js"></script>
    <script src="dash.js/src/streaming/vo/protection/KeyMessage.js"></script>
    <script src="dash.js/src/streaming/vo/protection/LicenseRequestComplete.js"></script>
    <script src="dash.js/src/streaming/vo/protection/NeedKey.js"></script>
    <script src="dash.js/src/streaming/vo/protection/ProtectionData.js"></script>
    <script src="dash.js/src/streaming/vo/protection/KeySystemAccess.js"></script>
    <script src="dash.js/src/streaming/vo/protection/KeySystemConfiguration.js"></script>
    <script src="dash.js/src/streaming/vo/protection/MediaCapability.js"></script>

    <!--Rules-->
    <script src="dash.js/src/streaming/rules/SwitchRequest.js"></script>
    <script src="dash.js/src/streaming/rules/RulesContext.js"></script>
    <script src="dash.js/src/streaming/rules/RulesController.js"></script>

    <script src="dash.js/src/streaming/rules/ABRRules/InsufficientBufferRule.js"></script>
    <script src="dash.js/src/streaming/rules/ABRRules/ABRRulesCollection.js"></script>
    <script src="dash.js/src/streaming/rules/ABRRules/BufferOccupancyRule.js"></script>
    <script src="dash.js/src/streaming/rules/ABRRules/ThroughputRule.js"></script>
    <script src="dash.js/src/streaming/rules/ABRRules/AbandonRequestsRule.js"></script>

    <script src="dash.js/src/streaming/rules/SchedulingRules/ScheduleRulesCollection.js"></script>
    <script src="dash.js/src/streaming/rules/SchedulingRules/BufferLevelRule.js"></script>
    <script src="dash.js/src/streaming/rules/SchedulingRules/PendingRequestsRule.js"></script>
    <script src="dash.js/src/streaming/rules/SchedulingRules/SameTimeRequestRule.js"></script>
    <script src="dash.js/src/streaming/rules/SchedulingRules/PlaybackTimeRule.js"></script>

    <script src="dash.js/src/streaming/rules/SynchronizationRules/LiveEdgeBinarySearchRule.js"></script>
    <script src="dash.js/src/streaming/rules/SynchronizationRules/LiveEdgeWithTimeSynchronizationRule.js"></script>
    <script src="dash.js/src/streaming/rules/SynchronizationRules/SynchronizationRulesCollection.js"></script>

    <!--Captioning-->
    <script src="dash.js/src/streaming/VTTParser.js"></script>
    <script src="dash.js/src/streaming/TTMLParser.js"></script>
    <script src="dash.js/src/streaming/TextSourceBuffer.js"></script>
    <script src="dash.js/src/streaming/extensions/TextTrackExtensions.js"></script>
    <script src="dash.js/src/streaming/controllers/TextController.js"></script>

    <!-- Metrics -->
    <script src="dash.js/src/streaming/models/MetricsModel.js"></script>
    <script src="dash.js/src/streaming/vo/MetricsList.js"></script>
    <script src="dash.js/src/streaming/vo/metrics/BufferLevel.js"></script>
    <script src="dash.js/src/streaming/vo/metrics/BufferState.js"></script>
    <script src="dash.js/src/streaming/vo/metrics/HTTPRequest.js"></script>
    <script src="dash.js/src/streaming/vo/metrics/PlayList.js"></script>
    <script src="dash.js/src/streaming/vo/metrics/RepresentationSwitch.js"></script>
    <script src="dash.js/src/streaming/vo/metrics/TCPConnection.js"></script>
    <script src="dash.js/src/streaming/vo/metrics/DroppedFrames.js"></script>
    <script src="dash.js/src/streaming/vo/metrics/SchedulingInfo.js"></script>
    <script src="dash.js/src/streaming/vo/metrics/RequestsQueue.js"></script>
    <script src="dash.js/src/streaming/vo/metrics/ManifestUpdate.js"></script>
    <script src="dash.js/src/streaming/vo/metrics/DVRInfo.js"></script>

    <!-- Dash -->
    <script src="dash.js/src/dash/Dash.js"></script>
    <script src="dash.js/src/dash/DashContext.js"></script>
    <script src="dash.js/src/dash/vo/Mpd.js"></script>
    <script src="dash.js/src/dash/vo/Period.js"></script>
    <script src="dash.js/src/dash/vo/AdaptationSet.js"></script>
    <script src="dash.js/src/dash/vo/Representation.js"></script>
    <script src="dash.js/src/dash/vo/Segment.js"></script>
    <script src="dash.js/src/dash/vo/Event.js"></script>
    <script src="dash.js/src/dash/vo/EventStream.js"></script>
    <script src="dash.js/src/dash/vo/UTCTiming.js"></script>
    <script src="dash.js/src/dash/DashParser.js"></script>
    <script src="dash.js/src/dash/DashHandler.js"></script>
    <script src="dash.js/src/dash/controllers/RepresentationController.js"></script>
    <script src="dash.js/src/dash/extensions/BaseURLExtensions.js"></script>
    <script src="dash.js/src/dash/extensions/FragmentExtensions.js"></script>
    <script src="dash.js/src/dash/extensions/DashManifestExtensions.js"></script>
    <script src="dash.js/src/dash/extensions/DashMetricsExtensions.js"></script>
    <script src="dash.js/src/dash/TimelineConverter.js"></script>
    <script src="dash.js/src/dash/DashAdapter.js"></script>

    <!-- App -->
    <script src="dash.js/samples/dash-if-reference-player/app/metrics.js"></script>
    <script src="dash.js/contrib/akamai/controlbar/ControlBar.js"></script>
    <script src="dash.js/samples/dash-if-reference-player/app/main.js"></script>

<script type="text/javascript">

var player;
var udchannel = 1;
var totalChannelChanges = 0;
var switchStartTime = 0;
var totalSwitchingDuration = 0;
var totalTuneinDuration = 0;
var fragmentLoadErrorCount = 0;

window.onload = function()
{  
    fragmentLoadErrorCount = 0;
	logger.clear();
    logger.log('Ready');
    var videoTag = document.querySelector("#videoPlayer");
    var context = new Dash.di.DashContext();
    player= new MediaPlayer(context);
    player.startup();
    player.attachView(videoTag);

    var video = document.getElementById('videoPlayer');
         

    function showEvent(msg) {
      //var elapsedTimeMs = Date.now() - startupTimeMs;
      //msg = elapsedTimeMs + ":" + msg + " ";
      //console.log("#### " + msg);
      //videoEventTag.value += msg;
    }
    videoTag.addEventListener('loadstart', function(video, e) {showEvent("loadstart");});
    videoTag.addEventListener('loadedmetadata', function(video, e) {showEvent("loadedmetadata");});
    videoTag.addEventListener('loadeddata', function(video, e) {showEvent("loadeddata");});
    videoTag.addEventListener('canplay', function(video, e) {showEvent("canplay");});
    videoTag.addEventListener('canplaythrough', function(video, e) {showEvent("canplaythrough");});
    videoTag.addEventListener('playing', playingNow);
    videoTag.addEventListener('progress', function(video, e) {
      // showEvent("progress");
    });

    setInterval(function () {monitor404Errors()}, 200);
    //contentListTag.addEventListener('change', function () {
     // console.log("### content changed");
     // videoTag.pause();
   // });
   
   //start(1);
   
}
 var waitStarted = 0;
 
function playEvent()
{
    fragmentLoadErrorCount = 0;
    waitStarted = 0;
}

window.onbeforeunload = function (e) {
    // Cleanup on exit
    $.post(
            "Cleanup.php",
            {channel:"1"},
            function(response)
            {
                console.log(response);
            }
           );
};

function playingNow(e)
{
	logger.clear();
    logger.log("Playing Channel " + udchannel);
    var timeNow = new Date();
    console.log("***************** Playback started, " + timeNow + timeNow.getMilliseconds());
    //switchChannel();
}

function switchChannel()
{
    totalChannelChanges ++;
    var timeNow = Date.now();
    totalSwitchingDuration += (timeNow - switchStartTime);
    var status =document.getElementById('switches');
    status.innerHTML="Total time in switches: " + (totalSwitchingDuration/1000).toFixed(2) + " sec, Switches: " + totalChannelChanges + ", " 
            + (totalSwitchingDuration/(totalChannelChanges*1000)).toFixed(2) + " seconds for switch, " 
            + (totalTuneinDuration/(totalChannelChanges*1000)).toFixed(2) + " seconds for tune in";

    var timeNow = Date.now();        
    var nextChangeDelay = Math.ceil(timeNow/1000)*1000 - timeNow + Math.random()*1000;
    if(udchannel == 1)
      setTimeout(function () {start(2)}, nextChangeDelay);
    else
      setTimeout(function () {start(1)}, nextChangeDelay);

    //videoTag.pause();
 }

function start(channel)
{
    var timeNow = new Date();
    switchStartTime = Date.now();
    console.log("****************************Starting Channel " + channel  + timeNow + timeNow.getMilliseconds() + " ****************************");
    var videoTag = document.querySelector("#videoPlayer");
    videoTag.pause();
    player.attachSource(null);

    udchannel = channel;

  //Make sure no flute process is running
  $.post(
          "Cleanup.php",
          {channel:channel},
          function(response)
          {
              console.log(response);
          }
        );

  // Start flute process for a channel
    $.post(
            "Process.php",
            {channel:channel},
            function(response)
            {
                totalTuneinDuration += (Date.now() - switchStartTime);
                if(response == "Started channel 1")
                  var localChannel = 1;
                else
                  var localChannel = 2;
                var DASHContentBase="DASH_Content";
                var DASHContentDir= DASHContentBase + localChannel;
                var patchedMPD="MultiRate_Dynamic_Patched.mpd";
                var scriptPath = document.URL;
                var scriptFolder = scriptPath.substr(0, scriptPath.lastIndexOf( '/' )+1 );
                var mpdURL = scriptFolder + DASHContentDir + "/" + patchedMPD;
                // By this response, MPD is found, start playback
                console.log(response);
				logger.clear();
				logger.log("Tuned in, buffering ...");				

                // Start player
                var timeNow = new Date();
                console.log("*********** Instructing to play MPD: " + mpdURL + timeNow + timeNow.getMilliseconds());
                player.attachSource(mpdURL);
                videoTag.play();
                
                //setTimeout(function () {window.open("http://localhost/Work/Route_Receiver/Receiver/dash.js/samples/dash-if-reference-player/index.html?url=" + mpdURL)}, 0);
                ;

            }
          );
		  
		logger.clear();
		logger.log("Tuning in ...");	
 }
 
 function monitor404Errors()
 {
     if(fragmentLoadErrorCount > 0 && waitStarted == 0)
     {
         //setTimeout(function () {start(udchannel)}, 500);
         waitStarted = waitStarted +1;
     }
     
     if(waitStarted > 0)
         waitStarted = waitStarted +1;
     
     if(waitStarted > (6/0.2))
         waitStarted = 0;
 }

 function Logger(id) {
  this.el = document.getElementById('log');
}
Logger.prototype.log = function(msg) {
  var fragment = document.createDocumentFragment();
  fragment.appendChild(document.createTextNode(msg));
  fragment.appendChild(document.createElement('br'));
  this.el.appendChild(fragment);
};

Logger.prototype.clear = function() {
  this.el.textContent = '';
};

var logger = new Logger('log');

logger.log('');

</script>