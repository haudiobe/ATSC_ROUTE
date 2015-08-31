<!DOCTYPE html>
<!--
Main script to start playback.
Provide the correct dash.js URL as "playerURL" below
Start a channel before
-->

<html>
    <head>
        <meta charset="UTF-8">
        <title style="font-family: Helvetica,Arial,sans-serif; font-weight: normal;">Route Demo</title>
        <link rel="stylesheet" type="text/css" href="stylecss.css">       
    </head>
    <body onLoad="onLoad()" id="demopage">
    	<div id="ptitle" class="cc01">
    		<div><font color="#e0e0e0"><font face="Trebuchet">ROUTE</font> DEMO</font> </div>
    	</div>
	    <button class="btnSty" id="channel1" >Channel 1</button>
        <button class="btnSty" id="channel2">Channel 2</button>
        <br>    <br> 
        <div style="font-family: Helvetica,Arial,sans-serif; font-weight: normal; font-size: 120%;" id="switches"></div>
        <div style="font-family: Helvetica,Arial,sans-serif; font-weight: normal; font-size: 120%;" id="status"></div>
        <br>
    	<div id="playerDiv">
    		<video width="960" height="540" id="videoPlayer" preload="" controls="controls">
    		</video>
    	</div>     
    </body>
</html>

<?php 
;
?>

<script src="jquery-1.11.1.min.js"></script>
    <!-- http://angularjs.org/ -->
    <script src="samples/dash-if-reference-player/app/lib/angular/angular.min.js"></script>
    <script src="samples/dash-if-reference-player/app/lib/angular/angular-resource.min.js"></script>

    <!-- http://getbootstrap.com/ -->
    <script src="samples/dash-if-reference-player/app/lib/bootstrap/js/bootstrap.min.js"></script>

    <!-- https://github.com/creynders/dijon-framework -->
    <script src="src/lib/dijon.js"></script>

    <!-- http://bannister.us/weblog/2007/06/09/simple-base64-encodedecode-javascript/ -->
    <script src="src/lib/base64.js"></script>

    <!-- Misc Libs -->
    <script src="src/lib/xml2json.js"></script>
    <script src="src/lib/objectiron.js"></script>
    <script src="src/lib/long.js"></script>
    <script src="src/lib/Math.js"></script>

    <!-- http://www.flotcharts.org/ -->
    <script src="samples/dash-if-reference-player/app/lib/flot/jquery.flot.js"></script>

    <!-- https://github.com/eu81273/angular.treeview -->
    <script src="samples/dash-if-reference-player/app/lib/angular.treeview/angular.treeview.min.js"></script>

    <!-- Player -->
    <script src="src/streaming/MediaPlayer.js"></script>
    <script src="src/streaming/Context.js"></script>
    <script src="src/streaming/ErrorHandler.js"></script>
    <script src="src/streaming/utils/Capabilities.js"></script>
    <script src="src/streaming/utils/EventBus.js"></script>
    <script src="src/streaming/utils/Debug.js"></script>
    <script src="src/streaming/extensions/RequestModifierExtensions.js"></script>
    <script src="src/streaming/models/VideoModel.js"></script>
    <script src="src/streaming/vo/FragmentRequest.js"></script>
    <script src="src/streaming/vo/TrackInfo.js"></script>
    <script src="src/streaming/vo/MediaInfo.js"></script>
    <script src="src/streaming/vo/StreamInfo.js"></script>
    <script src="src/streaming/vo/ManifestInfo.js"></script>
    <script src="src/streaming/vo/Event.js"></script>
    <script src="src/streaming/vo/Error.js"></script>
    <script src="src/streaming/ManifestLoader.js"></script>
    <script src="src/streaming/ManifestUpdater.js"></script>
    <script src="src/streaming/models/ManifestModel.js"></script>
    <script src="src/streaming/extensions/MediaSourceExtensions.js"></script>
    <script src="src/streaming/extensions/SourceBufferExtensions.js"></script>
    <script src="src/streaming/extensions/VideoModelExtensions.js"></script>
    <script src="src/streaming/controllers/PlaybackController.js"></script>
    <script src="src/streaming/controllers/FragmentController.js"></script>
    <script src="src/streaming/controllers/AbrController.js"></script>
    <script src="src/streaming/FragmentLoader.js"></script>
    <script src="src/streaming/models/FragmentModel.js"></script>
    <script src="src/streaming/controllers/StreamController.js"></script>
    <script src="src/streaming/StreamProcessor.js"></script>
    <script src="src/streaming/controllers/ScheduleController.js"></script>
    <script src="src/streaming/TimeSyncController.js"></script>
    <script src="src/streaming/Stream.js"></script>
    <script src="src/streaming/controllers/BufferController.js"></script>
    <script src="src/streaming/LiveEdgeFinder.js"></script>
    <script src="src/streaming/Notifier.js"></script>
    <script src="src/streaming/controllers/EventController.js"></script>
    <script src="src/streaming/models/URIQueryAndFragmentModel.js"></script>
    <script src="src/streaming/vo/URIFragmentData.js"></script>

    <!--Protection-->
    <script src="src/streaming/models/ProtectionModel.js"></script>
    <script src="src/streaming/models/ProtectionModel_01b.js"></script>
    <script src="src/streaming/models/ProtectionModel_3Feb2014.js"></script>
    <script src="src/streaming/vo/protection/SessionToken.js"></script>

    <script src="src/streaming/controllers/ProtectionController.js"></script>
    <script src="src/streaming/extensions/ProtectionExtensions.js"></script>
    <script src="src/streaming/protection/CommonEncryption.js"></script>

    <script src="src/streaming/protection/drm/KeySystem.js"></script>
    <script src="src/streaming/protection/drm/KeySystem_Access.js"></script>
    <script src="src/streaming/protection/drm/KeySystem_ClearKey.js"></script>
    <script src="src/streaming/protection/drm/KeySystem_PlayReady.js"></script>
    <script src="src/streaming/protection/drm/KeySystem_Widevine.js"></script>
    <script src="src/streaming/protection/drm/KeySystem.js"></script>

    <script src="src/streaming/vo/protection/KeyError.js"></script>
    <script src="src/streaming/vo/protection/KeyMessage.js"></script>
    <script src="src/streaming/vo/protection/LicenseRequestComplete.js"></script>
    <script src="src/streaming/vo/protection/NeedKey.js"></script>
    <script src="src/streaming/vo/protection/ProtectionData.js"></script>

    <!--Rules-->
    <script src="src/streaming/rules/SwitchRequest.js"></script>
    <script src="src/streaming/rules/RulesContext.js"></script>
    <script src="src/streaming/rules/RulesController.js"></script>

    <script src="src/streaming/rules/ABRRules/DownloadRatioRule.js"></script>
    <script src="src/streaming/rules/ABRRules/InsufficientBufferRule.js"></script>
    <script src="src/streaming/rules/ABRRules/LimitSwitchesRule.js"></script>
    <script src="src/streaming/rules/ABRRules/ABRRulesCollection.js"></script>
    <script src="src/streaming/rules/ABRRules/BufferOccupancyRule.js"></script>
    <script src="src/streaming/rules/ABRRules/ThroughputRule.js"></script>

    <script src="src/streaming/rules/SchedulingRules/ScheduleRulesCollection.js"></script>
    <script src="src/streaming/rules/SchedulingRules/BufferLevelRule.js"></script>
    <script src="src/streaming/rules/SchedulingRules/PendingRequestsRule.js"></script>
    <script src="src/streaming/rules/SchedulingRules/SameTimeRequestRule.js"></script>
    <script src="src/streaming/rules/SchedulingRules/PlaybackTimeRule.js"></script>

    <script src="src/streaming/rules/SynchronizationRules/LiveEdgeBinarySearchRule.js"></script>
    <script src="src/streaming/rules/SynchronizationRules/LiveEdgeWithTimeSynchronizationRule.js"></script>
    <script src="src/streaming/rules/SynchronizationRules/SynchronizationRulesCollection.js"></script>

    <!--Captioning-->
    <script src="src/streaming/VTTParser.js"></script>
    <script src="src/streaming/TTMLParser.js"></script>
    <script src="src/streaming/TextSourceBuffer.js"></script>
    <script src="src/streaming/extensions/TextTrackExtensions.js"></script>
    <script src="src/streaming/controllers/TextController.js"></script>

    <!-- Metrics -->
    <script src="src/streaming/models/MetricsModel.js"></script>
    <script src="src/streaming/vo/MetricsList.js"></script>
    <script src="src/streaming/vo/metrics/BufferLevel.js"></script>
    <script src="src/streaming/vo/metrics/BufferState.js"></script>
    <script src="src/streaming/vo/metrics/HTTPRequest.js"></script>
    <script src="src/streaming/vo/metrics/PlayList.js"></script>
    <script src="src/streaming/vo/metrics/RepresentationSwitch.js"></script>
    <script src="src/streaming/vo/metrics/TCPConnection.js"></script>
    <script src="src/streaming/vo/metrics/DroppedFrames.js"></script>
    <script src="src/streaming/vo/metrics/SchedulingInfo.js"></script>
    <script src="src/streaming/vo/metrics/ManifestUpdate.js"></script>
    <script src="src/streaming/vo/metrics/DVRInfo.js"></script>

    <!-- Dash -->
    <script src="src/dash/Dash.js"></script>
    <script src="src/dash/DashContext.js"></script>
    <script src="src/dash/vo/Mpd.js"></script>
    <script src="src/dash/vo/Period.js"></script>
    <script src="src/dash/vo/AdaptationSet.js"></script>
    <script src="src/dash/vo/Representation.js"></script>
    <script src="src/dash/vo/Segment.js"></script>
    <script src="src/dash/vo/Event.js"></script>
    <script src="src/dash/vo/EventStream.js"></script>
    <script src="src/dash/vo/UTCTiming.js"></script>
    <script src="src/dash/DashParser.js"></script>
    <script src="src/dash/DashHandler.js"></script>
    <script src="src/dash/controllers/RepresentationController.js"></script>
    <script src="src/dash/extensions/BaseURLExtensions.js"></script>
    <script src="src/dash/extensions/FragmentExtensions.js"></script>
    <script src="src/dash/extensions/DashManifestExtensions.js"></script>
    <script src="src/dash/extensions/DashMetricsExtensions.js"></script>
    <script src="src/dash/TimelineConverter.js"></script>
    <script src="src/dash/DashAdapter.js"></script>

    <!-- App -->
    <script src="samples/dash-if-reference-player/app/metrics.js"></script>
    <script src="samples/dash-if-reference-player/app/main.js"></script>

<script type="text/javascript">

var player;
var udchannel = 1;
var totalChannelChanges = 0;
var switchStartTime = 0;
var totalSwitchingDuration = 0;
var totalTuneinDuration = 0;

window.onload = function()
{  
    var status =document.getElementById('status');

    status.innerHTML="Ready";

    var videoTag = document.querySelector("#videoPlayer");
    var context = new Dash.di.DashContext();
    player= new MediaPlayer(context);
    player.startup();
    player.attachView(videoTag);

    var channel1item = document.querySelector("#channel1");
    var channel2item = document.querySelector("#channel2");
    var video = document.getElementById('videoPlayer');
         
    channel1item.addEventListener('click', function () {
       start(1);
    });
    channel2item.addEventListener('click', function () {
         start(2);
    });

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

    //contentListTag.addEventListener('change', function () {
     // console.log("### content changed");
     // videoTag.pause();
   // });
   
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
    var status =document.getElementById('status');

    status.innerHTML="Playing Channel " + udchannel;
    status.style.color = "black";
    status.style.fontWeight = "normal";
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
                var status =document.getElementById('status');
                status.innerHTML="Tuned in, buffering ...";
                status.style.color = "green";
                status.style.fontWeight = "bold";
                // Start player
                var timeNow = new Date();
                console.log("*********** Instructing to play MPD: " + mpdURL + timeNow + timeNow.getMilliseconds());
                player.attachSource(mpdURL);
                videoTag.play();

            }
          );

    var status =document.getElementById('status');
    status.innerHTML="Tuning in ...";
    status.style.color = "black";
    status.style.fontWeight = "normal";
 }
</script>