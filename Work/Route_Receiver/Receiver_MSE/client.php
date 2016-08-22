<!DOCTYPE html>
<!--
Copyright 2015

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Author: Waqar Zia

MSE Example used by Eric Bidelman (ebidel@)
-->
<html>
<head>
<meta charset="utf-8" />
<meta http-equiv="X-UA-Compatible" content="IE=Edge,chrome=1" />
<title>ROUTE MDE Delivery</title>
<link href='http://fonts.googleapis.com/css?family=Open+Sans' rel='stylesheet' type='text/css'>
<link href="style.css" rel="stylesheet">
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

</style>
</head>
<body>

<h3>ROUTE MDE Delivery</h3>

<div style="background-image: url(thumbs/background.png); height: 540px; width: 960px;">
  <video controls width="960" height="540" onplay="playEvent()" onerror="playbackFailed(event)"></video>
</div>
<section>
  <pre id="log"></pre>
</section>
  <div id="streamWrapper">
    <h2>Choose a Channel</h2>
    <div style="height: 150px; display:inline; vertical-align: middle;">
      <div id="thumbs" style="display:inline-flex;">
        <a href="#" title="Channel 1" onclick="PlayChannel(1);return false;"><img src="thumbs/thumb1.png" /></a>
        <a href="#" title="Channel 2" onclick="PlayChannel(2);return false;"><img src="thumbs/thumb2.png" /></a>
      </div>
    </div>
  </div> 

  <p id="AdSel">Unicast</p> 
  <div class="onoffswitch">
    <input onchange="checkthis(this)" type="checkbox" name="onoffswitch" class="onoffswitch-checkbox" id="myonoffswitch" checked>
      <label class="onoffswitch-label" for="myonoffswitch">
      <span class="onoffswitch-inner"></span>
      <span class="onoffswitch-switch"></span>
      </label>
  </div>

  
<script src="jquery-1.11.1.min.js"></script>
<script>
var video = document.querySelector('video');

window.onload = function()
{  	// Once the MSE receiver window is loaded in the browser. 	
	// Start the python script to receive the SLT signalling.
	// The script will put the SLT.xml file in the SLT_signalling folder.
	  $.ajax({
			  type: 'POST',
			  url: "ReceiverConfig/onloadfunc.php",
			  datatype: "json",
	});
}

window.MediaSource = window.MediaSource || window.WebKitMediaSource;
if (!!!window.MediaSource) {
  alert('MediaSource API is not available');
}
var mediaSource;
var logString;
var startTime;
var numSwitches = 0;
var sumChangeTime = 0;
var startTimeTotal = 0;
var sourceBuffersReady = true; // Can I already start appending to the source buffer?
var sourceBuffer;
var audioSourceBuffer;
var initVideoBuffer = true;
var autoPlaybackDone = false;
var result;
var reTuneOver1 = false;
var reTuneOver2 = false;
var tuneinTriggeredFromVideo = false;
var tuneinTriggeredFromAudio = false;
var customAd = false;
var customAdTriggered = false;
var reTuneInVideo = 0;
var reTuneInAudio = 0;
var customAdOffset = 10;

function checkthis(ele){   
  if(!ele.checked )
    customAd = true;
    else
    customAd = false;
}


function playbackFailed(e) {
  // Video playback failed - show a message saying why.
  // Useful for the purpose of debugging.
   switch (e.target.error.code) {
     case e.target.error.MEDIA_ERR_ABORTED:
       alert('You aborted the video playback.');
       break;
    case e.target.error.MEDIA_ERR_NETWORK:
      alert('A network error caused the video download to fail part-way.');
      break;
   case e.target.error.MEDIA_ERR_DECODE:
      alert('The video playback was aborted due to a corruption problem or because the video used features your browser did not support.');
      break;
   case e.target.error.MEDIA_ERR_SRC_NOT_SUPPORTED:
     alert('The video could not be loaded, either because the server or network failed or because the format is not supported.');
     break;
   default:
     alert('An unknown error occurred.');
     break;
   }
}

function reInitializeVariables(){
 // Since, we changed the scope of many variables to global. We need to reintialize them when
 // we switch channels. This way they go back to the original state. 
 sourceBuffersReady = true; // The sourcebuffers (audio and video) are ready to be appended.
 initVideoBuffer = true;
 autoPlaybackDone = false;
 tuneinTriggeredFromVideo = false; // Indicates whether the .init segment was first received from
 tuneinTriggeredFromAudio = false; // video or audio.
}

function PlayChannel(channel)
{
    $.post(
          "Cleanup.php",
          {channel:channel},
          function(response)
          {
              console.log(response);
          }
        );

  $.post(
	  "Process.php",
	  {channel:channel},
	  function(response)
	  {
	    result = JSON.parse(response); 
	    var localChannel = result[0]; 	
	    reInitializeVariables();	
	    mediaSource = new MediaSource();                

	    // Now the mediasource object takes place of a file URL for the 
	    // src of the video elements. 
	    // And the video is now pointing at the mediasource object.
	    video.src = window.URL.createObjectURL(mediaSource);
	    
	    // Wait for event that tells us that our media source object is 
	    // ready for a buffer to be added.	  	    
	    mediaSource.addEventListener('sourceopen', callback, false);
	
	    // Adi : Yet to figure out what this eventListener does? 
	    // Adi : Proabaly this implies, either sourceopen or webkitsourceopen is called. 
	    mediaSource.addEventListener('webkitsourceopen', callback, false);
                
	    {
	      var tt = new Date;
	      console.log("Added MSE: " + tt + tt.getMilliseconds());
	    }

	  }
	);
    var tt = new Date;
    console.log("*****Channel change start: " + tt + tt.getMilliseconds());
    startTime = new Date();
    if(numSwitches == 0)
      startTimeTotal = startTime;
}

/**
 * Creates a new Uint8Array based on two different ArrayBuffers
 *
 * @private
 * @param {ArrayBuffers} buffer1 The first buffer.
 * @param {ArrayBuffers} buffer2 The second buffer.
 * @return {ArrayBuffers} The new ArrayBuffer created out of the two.
 */
var _appendBuffer = function(buffer1, buffer2) {
  var tmp = new Uint8Array(buffer1.byteLength + buffer2.byteLength);
  tmp.set(new Uint8Array(buffer1), 0);
  tmp.set(new Uint8Array(buffer2), buffer1.byteLength);
  return tmp;
};

function playEvent()
{
    var timeNow = new Date;
    var channelChangeDuration = timeNow - startTime;
    console.log("Play event called: " + timeNow + timeNow.getMilliseconds());
    sumChangeTime = sumChangeTime + channelChangeDuration;
    numSwitches = numSwitches + 1;
    logger.clear();
    logger.log('Last channel change time: ' + channelChangeDuration + " msec, Average: " + (sumChangeTime/numSwitches).toFixed(2) + " msec");
}

function callbackNew(e)
{
	  sourceBuffer = mediaSource.addSourceBuffer('video/mp4; codecs="avc1.640028"');
	  audioSourceBuffer = mediaSource.addSourceBuffer('audio/mp4; codecs="mp4a.40.2"');
	  sourceBuffersReady = true;
	  //audioSourceBuffer.addEventListener("error",callbackErrorInBuffer,true);
	  //audioSourceBuffer.addEventListener("update",callbackErrorInBuffer2,true);
	  //audioSourceBuffer.addEventListener("abort",callbackErrorInBuffer3,true);
	  console.log("New buffers created");
	}

function callbackErrorInBuffer(e){
	console.log("Some errors in source buffer");
}

function callbackErrorInBuffer2(e){
	console.log("Some errors in source buffer - update");
}

function callbackErrorInBuffer3(e){
	console.log("Some errors in source buffer - abort");
}

function callbackErrorInPlayback(e){
	console.log("Some errors in playback.");
}



function reTuneInProcess(triggeredFrom){
  console.log("Tunein triggered from " + triggeredFrom);	
  console.log("Now will attempt to remove buffer and add it again" );
  // Ideally, one should be able to remove the buffers of mediasource and add it again.
  // But there have been issues in realizing this in some of the buffers.
  // For detailed description see : https://github.com/Dash-Industry-Forum/dash.js/issues/126
  // The way to do this now is point the video element to null. 
  // What this does is remove the entire mediasource element assoicated with the video element.
  // This is a routine which is executed when the video element is pointed to null.  
  if (customAd && !reTuneOver2){
    video.src = "../Receiver_MSE/CustomAd/video_30s.mp4#t=" + customAdOffset;
    /*
    Adjust video start and end time when using the video tag in html5;
    http://www.yoursite.com/yourfolder/yourfile.mp4#t=5,15
    where left of comma is start time in seconds, right of comma is end time in seconds. drop the comma and end time to effect the start time only.
    */
    video.play();
    video.addEventListener('ended',videoEnded,false);
    customAdTriggered = true;
  }
  else{  
    video.src = null;
    sourceBuffersReady = false; // The sourcebuffers are not yet created.    
    if (triggeredFrom == "video")
      tuneinTriggeredFromVideo = true; // Later used to discard packets until audio init comes.
    else 
      tuneinTriggeredFromAudio = true; // Later used to discard packets until video init comes.  
    // Repeat the earlier process.
    mediaSource = new MediaSource();
    video.src = window.URL.createObjectURL(mediaSource);
    mediaSource.addEventListener('sourceopen', callbackNew, false);
    mediaSource.addEventListener('webkitsourceopen', callbackNew, false);
    video.addEventListener('error', callbackErrorInPlayback, false);  // Indicates if any error in playback.
    autoPlaybackDone = false;
  }
}

function videoEnded(e){
  console.log("The custom ad video has finished playing.");
  customAdTriggered = false;
  reTuneInVideo = 1;
  reTuneOver2 = true;
}


function callback(e)
{
  var tt = new Date;
  console.log("MSE callback started: " + tt + tt.getMilliseconds());
  sourceBuffer = mediaSource.addSourceBuffer('video/mp4; codecs="avc1.640028"');
  audioSourceBuffer = mediaSource.addSourceBuffer('audio/mp4; codecs="mp4a.40.2"');
  logString = "";
  
  var boundary1 = result[3]*1000; 
  var boundary2 = (result[4]-result[3])*1000; 
  var deltaTimeASTTuneIn = result[1];    
  // Using the deltaTimeASTTuneIn, you will approximately know which place you are in.
    
  if ((deltaTimeASTTuneIn > result[3]) && (deltaTimeASTTuneIn < result[4])){
    reTuneOver1 = true;
    if (customAd){
      reTuneInProcess("Callback");
      customAdOffset = deltaTimeASTTuneIn - result[3] - 2; 
      // The -2 is just a safe number so that we dont run out of ad and create some error.
      // This value will be updated or changed in the future.
      // How much ad has to be skipped?
      // -------------------------------> deltaTimeASTTuneIn
      // ---------------> result[3]
      //                 ---------------> customAdOffset
      console.log("Going to play custom ad with an offset of = " + customAdOffset);
    }
    
    }
  else if (deltaTimeASTTuneIn > result[4]){
    reTuneOver2 = true;   
    }
  
  //'mediaSource readyState: ' + this.readyStatevar period + "\n";
  //logger.log(logString);
  window.WebSocket = window.WebSocket || window.MozWebSocket;
  
  // ---------------------- Video websocket start---------------//	
  var websocket = new WebSocket('ws://127.0.0.1:9000',
			'dumb-increment-protocol');
  websocket.onopen = function () {
		  websocket.send("video");
  };
  websocket.onerror = function () {
      setTimeout(function(){ websocket = new WebSocket('ws://127.0.0.1:9000','dumb-increment-protocol'); }, 50);
  };
  // ---------------------- Video websocket end-----------------//
  
  // ---------------------- Audio websocket start---------------//
  var websocketAudio = new WebSocket('ws://127.0.0.1:9001',
				'dumb-increment-protocol');

  websocketAudio.onopen = function () {
		  websocketAudio.send("audio");
  };


  websocketAudio.onerror = function () {
      setTimeout(function(){ websocketAudio = new WebSocket('ws://127.0.0.1:9001','dumb-increment-protocol'); }, 50);
  };  
  // ---------------------- Audio websocket end---------------//

  // Convert an integer to a string made up of the bytes in network/big-endian order.
  
function htonl(n)
{
      var newv = ((n & 0xFF000000) >>> 24) + ((n & 0x00FF0000) >>> 8) + ((n & 0x0000FF00) <<  8) + ((n & 0x000000FF) <<  24);
	  return newv;
}

var PTOFound = 0;
var videoPTOFound = false;
var audioPTOFound = false;
var videoBuffer;
var audioBuffer;
var initAudioBuffer = true;
var minVideoAppendLength = 41000;
var minAudioAppendLength = 1000;
var lastAppendTime = 0;
var lastAppendTimeAudio = 0;
var tempSegmentCount = 0;	
// The subraction because the sourcebuffer get reinitiliazed to zero.  
var initInterval = 150; // We look for 150ms up and down around the boundary, for .init segment to arrive.
var sourceBufferLength = 0;
var audioSourceBufferLength = 0;
reTuneInVideo = 0;
reTuneInAudio = 0;



// ------------------ Video packet received from websocket -----------------//
websocket.onmessage = function (message)
{

	if (customAdTriggered)
	  return;
	  
	tempSegmentCount = tempSegmentCount + 1;					
	var arraybuffer;
	var arrayData;			
	var fileReader = new FileReader();
	fileReader.onload = function() 
	{
		arraybuffer = this.result;
		arrayData = new Uint8Array(arraybuffer);

		/* What this essentially does is this after if the tunein is triggered from audio, it waits 
		until we receive the first .init segment of video is received. It discards all other packets
		that were received in between the time of tuning in from the audio and until the first .init segment
		of video is received.
		*/ 
		if (tuneinTriggeredFromAudio){	
		  if ( (arrayData.length == 900) || (arrayData.length == 920) ){
		    initVideoBuffer = true;
		    tuneinTriggeredFromAudio = false;
		    console.log('Heya! video');
		    }
		  else 
		    return;
		}				
		//console.log("Video array data length = " + arrayData.length);
		
		// Checks for condition to reTune at the first boundary between end of first period and begining of ad.
		// The sourceBuffer.buffered indicates a portion of the media that has been buffered.
		// The sourceBufferLength (sourceBuffer.buffered.end) clearly indicates the end time of the media time that has been buffered.
		// This variable is a much better indication where we are in the time range than the other ones. 
		
		
		if( (sourceBufferLength < (boundary1 + initInterval)) && (sourceBufferLength > (boundary1 - initInterval)) && !reTuneOver1 ){
		  //console.log("Retuned 1");
		  reTuneInVideo = 1;
		  reTuneOver1 = true;
		}
		
		// Checks for condition to reTune at the second boundary between end of ad and begining of third period.
		if( (sourceBufferLength < (boundary2 + initInterval)) && (sourceBufferLength > (boundary2 - initInterval)) && !reTuneOver2 && reTuneOver1 ){
		  //console.log("Retuned 2");
		  reTuneInVideo = 1;
		  reTuneOver2 = true;
		}
		
		if ( reTuneInVideo && ((arrayData.length == 900) || (arrayData.length == 920)) ){
		  //Waits for the first .init segment to arrive after tunein state is triggered.
		  //TODO: The way we look for .init segment right now is for the size. This is ugly and hardcoded.
		  // In future, some other way shall be looked at. 
		  reTuneInVideo = 0;
		  reTuneInProcess("video");
		  initVideoBuffer = true;
		  videoPTOFound = false;
		  lastAppendTime = 0;
		}				
		
		if(initVideoBuffer == true){
			videoBuffer = arrayData;
			initVideoBuffer = false;				
		}
		else{					
			//console.log("Array data length 2 = " + arrayData.length);
			videoBuffer = _appendBuffer(videoBuffer,arrayData);
		}
		
	    var period = 0;
	    var timeNow = new Date();
	    if(lastAppendTime != 0)
	    {
		period = timeNow - lastAppendTime;
	    }
	    else
	    lastAppendTime = timeNow;
	
	    if(period > 100 && sourceBuffersReady){
	      initVideoBuffer = true;
	      sourceBuffer.appendBuffer(videoBuffer);
	      lastAppendTime = new Date();	
	      if(!autoPlaybackDone)
		console.log("Appending video buffer length: " + videoBuffer.byteLength + ", time: " + lastAppendTime + lastAppendTime.getMilliseconds());
	    }
	      
	    if (sourceBuffersReady) {					
	      if (sourceBuffer.buffered.length > 0) {
		sourceBufferLength = sourceBuffer.buffered.end(0) * 1000;
		if(videoPTOFound == false && sourceBuffer.buffered.start(0) > PTOFound) {
			PTOFound = sourceBuffer.buffered.start(0);
			video.currentTime = PTOFound;
			console.log("PTO found = " + PTOFound);
			videoPTOFound = true;
			var tt = new Date;
			console.log("Video PTO found: " + tt + tt.getMilliseconds());
		}

	      if(!autoPlaybackDone) {
		var tt = new Date;
		console.log("Play called from video: " + tt + tt.getMilliseconds());
		video.play(); // Start playing after 1st chunk is appended.
		autoPlaybackDone = true;
	      }
	    }
	  }
	//logger.clear();
	//logger.log(logString + 'Appending audio and video chunk ');
	};
      fileReader.readAsArrayBuffer(message.data);	
      var elem = document.getElementById('field_name');
      var msgpt1 = ", Size: " + message.data.size;
};
		
websocketAudio.onmessage = function (message) {
      if (customAdTriggered)
	return;
	


      var arraybuffer;
      var arrayData;
      var fileReader = new FileReader();
      fileReader.onload = function() {
	  arraybuffer = this.result;
	  arrayData = new Uint8Array(arraybuffer);

      /* What this essentially does is this after if the tunein is triggered from video, it waits 
      until we receive the first .init segment of audio is received. It discards all other packets
      that were received in between the time of tuning in from the video and until the first .init segment
      is received.
      */ 
      if (tuneinTriggeredFromVideo){	
	if (arrayData.length == 826){
	  initAudioBuffer = true;
	  tuneinTriggeredFromVideo = false;
	  console.log('Heya! audio');
	  }
	else 
	  return;			
      }

      if( (audioSourceBufferLength < (boundary1 + initInterval)) && (audioSourceBufferLength > (boundary1 - initInterval)) && !reTuneOver1 ){
	  console.log("Retuned audio 1");
	  reTuneInAudio = 1;
	  reTuneOver1 = true;
      }
	
      // Condition to reTune at the second boundary between end of ad and begining of third period.
      if( (audioSourceBufferLength < (boundary2 + initInterval)) && (audioSourceBufferLength > (boundary2 - initInterval)) && !reTuneOver2 && reTuneOver1 ) {
	console.log("Retuned audio 2");
	reTuneInAudio = 1;
	reTuneOver2 = true;
      }
	  
      if ( reTuneInAudio && (arrayData.length == 826) ){
	//Waits for the first .init segment to arrive after tunein state is triggered;
	reTuneInAudio = 0;
	reTuneInProcess("audio");
	initAudioBuffer = true;
	audioPTOFound = false;
	lastAppendTimeAudio = 0;
      }							     
	  
      if(initAudioBuffer == true){
	audioBuffer = arrayData;
	initAudioBuffer = false;
      }
      else{
	audioBuffer = _appendBuffer(audioBuffer,arrayData);
      }
	  
      var period = 0;
      var timeNow = new Date();
      if(lastAppendTimeAudio != 0)
      {
	period = timeNow - lastAppendTimeAudio;
      }
      else
	lastAppendTimeAudio = timeNow;

      if(period > 100 && sourceBuffersReady)
      {
	initAudioBuffer = true;
	audioSourceBuffer.appendBuffer(audioBuffer);
	lastAppendTimeAudio = new Date();
	if(!autoPlaybackDone)
	  console.log("Appending audio buffer length: " + audioBuffer.byteLength + ", time: " + lastAppendTimeAudio + lastAppendTimeAudio.getMilliseconds());
      }
	      
      if (sourceBuffersReady){				
	      if (audioSourceBuffer.buffered.length > 0) 
	      {
		      audioSourceBufferLength = audioSourceBuffer.buffered.end(0) * 1000;
			if(audioPTOFound == false && audioSourceBuffer.buffered.start(0) > PTOFound)
		      {
			      PTOFound = audioSourceBuffer.buffered.start(0);
			      video.currentTime = PTOFound;
										      
			      audioPTOFound = true;
			      {
				      var tt = new Date;
				      console.log("Audio PTO found: " + tt + tt.getMilliseconds());
			      }
		      }
			
		      if(!autoPlaybackDone)
		      {
			      video.play(); // Start playing after 1st chunk is appended.
			      autoPlaybackDone = true;
			      {
				      var tt = new Date;
				      console.log("Play called from Audio: " + tt + tt.getMilliseconds());
			      }
		      }
	      }
      }
      };
fileReader.readAsArrayBuffer(message.data);			
var msgpt1 = ", Size: " + message.data.size;
};
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
      
</script>
<script>
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
<script>
var _gaq = _gaq || [];
_gaq.push(['_setAccount', 'UA-22014378-1']);
_gaq.push(['_trackPageview']);

(function() {
  var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
  ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
  var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
})();
</script>
<!--[if IE]>
<script src="http://ajax.googleapis.com/ajax/libs/chrome-frame/1/CFInstall.min.js"></script>
<script>CFInstall.check({mode: 'overlay'});</script>
<![endif]-->
</body>
</html>



