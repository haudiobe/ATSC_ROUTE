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
<link href="style.css" rel="stylesheet">
<link href='http://fonts.googleapis.com/css?family=Open+Sans' rel='stylesheet' type='text/css'>
</head>

    <body id="demopage">
	<h3>ROUTE Demo</h3>
	
	<div id="video-container" style="background-image: url(thumbs/background.png); height: 540px; width: 960px;">
		<!-- Video -->
		<video id="video" width="960" height="540" onplay="playEvent()">
		  <p>
		    Your browser doesn't support HTML5 video.
		  </p>
		</video>
		<!-- Video Controls -->
		<div id="video-controls">
			<button type="button" id="mute" class="icon-volume-2"></button>
			<input type="range" id="volume-bar" min="0" max="1" step="0.1" value="1">
			<button type="button" id="full-screen" class="icon-fullscreen-alt"></button>
			<img src="thumbs/Progress_bar.gif" id="progress" alt="Status" style="width:780px;height:12px;">
		</div>
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

		<a alt="Settings" id="image" href="ReceiverConfig/index.php"><img height="42" width="42" src="thumbs/settings.jpg" /></a>
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
var video = null;
var fullScreen = false;
var muteButton;
var fullScreenButton;
var volumeValue;


window.onload = function()
{  
    fragmentLoadErrorCount = 0;
	logger.clear();
    logger.log('Ready');
	
	//Get IP
	  $.ajax({
			  type: 'POST',
			  url: "ReceiverConfig/onloadfunc.php",
			  datatype: "json",
	}).done( function(e) {
	  alternateServerIP = JSON.parse(e);
	  alternateLocation1 = "http://" + alternateServerIP + alternateLocation1;
	  alternateLocation2 = "http://" + alternateServerIP + alternateLocation2;
	  $.ajax({
                      type: 'POST',
                      url: "ReceiverConfig/setIP.php",
                       datatype: "json",
                      data: {ip: JSON.stringify(alternateServerIP)},
            }).done( function(e) {
               console.log("**** Setting IP Address and process, result: " + e);
                
            })
	});
	
		// Video
	video = document.getElementById("video");
    var context = new Dash.di.DashContext();
    player= new MediaPlayer(context);
    player.startup();
    player.attachView(video);
	// Buttons
	var playButton = document.getElementById("play-pause");
	muteButton = document.getElementById("mute");
	fullScreenButton = document.getElementById("full-screen");

	// Sliders
	var volumeBar = document.getElementById("volume-bar");
	volumeValue = volumeBar.value;
    var img = document.getElementById('progress');
	img.style.visibility = 'hidden';

	// Event listener for the mute button
	muteButton.addEventListener("click", function() {
		if (video.muted == false) {
			// Mute the video
			video.muted = true;
                muteButton.classList.add('icon-volume');
                muteButton.classList.remove('icon-volume-2');
				video.volume = volumeBar.value = 0;				
			// Update the button text
			//muteButton.innerHTML = "Unmute";
		} else {
			// Unmute the video
			video.muted = false;
                muteButton.classList.add('icon-volume-2');
                muteButton.classList.remove('icon-volume');
				video.volume = volumeBar.value = volumeValue;
			// Update the button text
			//muteButton.innerHTML = "Mute";
		}
	});


	// Event listener for the full-screen button
	fullScreenButton.addEventListener("click", function() {
		if(!fullScreen)
		{
			if (video.requestFullscreen) {
				video.requestFullscreen();
			} else if (video.mozRequestFullScreen) {
				video.mozRequestFullScreen(); // Firefox
			} else if (video.webkitRequestFullscreen) {
				video.webkitRequestFullscreen(); // Chrome and Safari
			}
			fullScreen = true;
                fullScreenButton.classList.remove('icon-fullscreen-alt');
                fullScreenButton.classList.add('icon-fullscreen-exit-alt');			
		}
		else
		{
			if (video.exitFullscreen) {
				video.exitFullscreen();
			} else if (video.mozCancelFullScreen) {
				video.mozCancelFullScreen(); // Firefox
			} else if (video.webkitExitFullscreen) {
				video.webkitExitFullscreen(); // Chrome and Safari
			}
			fullScreen = false;
                fullScreenButton.classList.add('icon-fullscreen-alt');
                fullScreenButton.classList.remove('icon-fullscreen-exit-alt');			
		}			
	});
	
	// Event listener for the volume bar
	volumeBar.addEventListener("change", function() {
		// Update the video volume
		volumeValue = video.volume = volumeBar.value;
	});

    function showEvent(msg) {
      //var elapsedTimeMs = Date.now() - startupTimeMs;
      //msg = elapsedTimeMs + ":" + msg + " ";
      //console.log("#### " + msg);
      //videoEventTag.value += msg;
    }
    video.addEventListener('loadstart', function(video, e) {showEvent("loadstart");});
    video.addEventListener('loadedmetadata', function(video, e) {showEvent("loadedmetadata");});
    video.addEventListener('loadeddata', function(video, e) {showEvent("loadeddata");});
    video.addEventListener('canplay', function(video, e) {showEvent("canplay");});
    video.addEventListener('canplaythrough', function(video, e) {showEvent("canplaythrough");});
    video.addEventListener('playing', playingNow);
    video.addEventListener('progress', function(video, e) {
      // showEvent("progress");
    });

    //contentListTag.addEventListener('change', function () {
     // console.log("### content changed");
     // video.pause();
   // });
   
   //start(1);
   
}

function playEvent()
{
    ;//fragmentLoadErrorCount = 0;
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
    var img = document.getElementById('progress');
    img.style.visibility = 'visible';	
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

    //video.pause();
 }
 
var alternateServerIP = "";
var alternateLocation1 = "/Work/Route_Sender/bin/ToS_1_0";
var alternateLocation2 = "/Work/Route_Sender/bin/Elysium_1_0";
var monitoringInterval = 200;
var monitoringWindowSize = 6;
var monitoringTime = 0;
var monitorProcess = null;
var fragmentErrorRegistry = [];
var reTuneAtAd = false;
var reTuneProcess = [];

function start(channel)
{
    var timeNow = new Date();
    switchStartTime = Date.now();
    console.log("****************************Starting Channel " + channel  + timeNow + timeNow.getMilliseconds() + " ****************************");
    var video = document.getElementById("video");
    video.pause();
    player.attachSource(null);
    var img = document.getElementById('progress');
	img.style.visibility = 'hidden';

    udchannel = channel;
	
	
	for(var index = 0 ; index < reTuneProcess.length ; index++)
	{
		 clearTimeout(reTuneProcess[index]);
	}
	
	reTuneProcess = [];

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
				result = JSON.parse(response);
                
				var localChannel = result[0];

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
                video.play();
                //setTimeout(function () {window.open("dash.js/samples/dash-if-reference-player/index.html?url=" + mpdURL)}, 0);
				monitoringTime = 0;
				monitorProcess = setInterval(function () {monitor404Errors()}, monitoringInterval);
				fragmentErrorRegistry = [];
				fragmentLoadErrorCount = 0;
				
				if(reTuneAtAd && result[1] > 0)
				{
					for(var index = 0, processStarted = 0 ; index < (result.length - 2) ; index++)
					{
						if(result[index+2] == 0)
							continue;
						
						reTuneProcess[processStarted] = setTimeout(function () {replay(mpdURL)}, result[index+2]*1000);
						processStarted = processStarted + 1;
					}
				}

            }
          );
		  
		logger.clear();
		logger.log("Tuning in ...");	
 }
 
 function monitor404Errors()
 {
	 monitoringTime += monitoringInterval;
	 fragmentErrorRegistry.push(fragmentLoadErrorCount);
	 fragmentLoadErrorCount = 0;
	 var maxWindowLength = monitoringWindowSize*(1000/monitoringInterval);
	 if(fragmentErrorRegistry.length > maxWindowLength)
		fragmentErrorRegistry.shift();
	var ErrorTH = 1;
	if(((monitoringTime/2000) - Math.floor(monitoringTime/2000))  < 0.1)
		console.log("*** Time: " + (monitoringTime/1000) + ", Errors: " + fragmentErrorRegistry.reduce(function(pv, cv) { return pv + cv; }, 0) + ", TH: " + monitoringWindowSize*ErrorTH + ", Result: " + (fragmentErrorRegistry.reduce(function(pv, cv) { return pv + cv; }, 0) > monitoringWindowSize*ErrorTH));
	if(fragmentErrorRegistry.reduce(function(pv, cv) { return pv + cv; }, 0) > monitoringWindowSize*ErrorTH) 
	 {
		 setTimeout(function () {start(udchannel)}, 0);
		 monitoringTime = 0;
		 clearInterval(monitorProcess);
		 console.log("************************ Triggering Re-tunein");
	 }
     
 }
 function replay(mpdURL)
 {
    var video = document.getElementById("video");
    video.pause();
    player.attachSource(null);	
	var timeNow = new Date();
	console.log("*********** Replaying MPD: " + mpdURL + timeNow + timeNow.getMilliseconds());
	player.attachSource(mpdURL);
	video.play();	
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