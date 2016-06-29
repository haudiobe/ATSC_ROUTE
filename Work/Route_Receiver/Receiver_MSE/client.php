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
  <video controls width="960" height="540" onplay="playEvent()"></video>
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

<script src="jquery-1.11.1.min.js"></script>
<script>
var video = document.querySelector('video');

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
var queue=[]; 
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
	      if(response == "Started channel 1")
		localChannel = 1;
	      else
		localChannel = 2;
		
         mediaSource = new MediaSource();

        video.src = window.URL.createObjectURL(mediaSource);
        mediaSource.addEventListener('sourceopen', callback, false);
        mediaSource.addEventListener('webkitsourceopen', callback, false);
	{
	    var tt = new Date;
        console.log("Added MSE: " + tt + tt.getMilliseconds());
    }

	  }
	);
	{
	    var tt = new Date;
        console.log("*****Channel change start: " + tt + tt.getMilliseconds());
    }

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

function callback(e)
{
    {
        var tt = new Date;
        console.log("MSE callback started: " + tt + tt.getMilliseconds());
    }

  var sourceBuffer = mediaSource.addSourceBuffer('video/mp4; codecs="avc1.640028"');
  var audioSourceBuffer = mediaSource.addSourceBuffer('audio/mp4; codecs="mp4a.40.2"');

  logString = "";//'mediaSource readyState: ' + this.readyState + "\n";

  //logger.log(logString);

  window.WebSocket = window.WebSocket || window.MozWebSocket;

        var websocket = new WebSocket('ws://127.0.0.1:9001',
                                      'dumb-increment-protocol');
									  
        var websocketAudio = new WebSocket('ws://127.0.0.1:9002',
                                      'dumb-increment-protocol');

        websocket.onopen = function () {
			websocket.send("video");
        };

        websocket.onerror = function () {
            setTimeout(function(){ websocket = new WebSocket('ws://127.0.0.1:9001','dumb-increment-protocol'); }, 50);
        };
	
        websocketAudio.onopen = function () {
			websocketAudio.send("audio");
        };

        websocketAudio.onerror = function () {
            setTimeout(function(){ websocketAudio = new WebSocket('ws://127.0.0.1:9002','dumb-increment-protocol'); }, 50);
        };
		
		// Convert an integer to a string made up of the bytes in network/big-endian order.
		function htonl(n)
		{
		     var newv = ((n & 0xFF000000) >>> 24) + ((n & 0x00FF0000) >>> 8) + ((n & 0x0000FF00) <<  8) + ((n & 0x000000FF) <<  24);
			 return newv;
		}
		
		var autoPlaybackDone = false;
		var PTOFound = 0;
		var videoPTOFound = false;
		var audioPTOFound = false;
		var videoBuffer;
		var initVideoBuffer = true;
		var audioBuffer;
		var initAudioBuffer = true;
		var minVideoAppendLength = 41000;
		var minAudioAppendLength = 1000;
		var lastAppendTime = 0;
		var lastAppendTimeAudio = 0;

        websocket.onmessage = function (message)
		{
			var arraybuffer;
			var arrayData;
			var fileReader = new FileReader();
			fileReader.onload = function() 
			{
			    arraybuffer = this.result;
			    //console.log(this.result);
				arrayData = new Uint8Array(arraybuffer);
				if(initVideoBuffer == true)
				{
					videoBuffer = arrayData;
                    initVideoBuffer = false;				
                }
				else
				{					
					videoBuffer = _appendBuffer(videoBuffer,arrayData);
				}
				//console.log(videoBuffer);
			    var period = 0;
			    var timeNow = new Date();
			    if(lastAppendTime != 0)
			    {
			        period = timeNow - lastAppendTime;
			        //console.log(period);
		        }
		        else
		            lastAppendTime = timeNow;
		        
				if(period > 100)
				{
					initVideoBuffer = true;
					console.log("Video source buffer");
				    console.log(sourceBuffer);
			//if (!sourceBuffer.updating )
			     // queue.push(videoBuffer);
			//else
			
			        sourceBuffer.appendBuffer(videoBuffer);
			        
    		        lastAppendTime = new Date();	
 					if(!autoPlaybackDone)
                        console.log("Appending video buffer length: " + videoBuffer.byteLength + ", time: " + lastAppendTime + lastAppendTime.getMilliseconds());

				}
				
				if (sourceBuffer.buffered.length > 0) 
				{
					if(videoPTOFound == false && sourceBuffer.buffered.start(0) > PTOFound)
					{
						PTOFound = sourceBuffer.buffered.start(0);
						video.currentTime = PTOFound;
						videoPTOFound = true;
						{
						    var tt = new Date;
                            console.log("Video PTO found: " + tt + tt.getMilliseconds());
                        }
					}

					if(!autoPlaybackDone)
					{
						{
						    var tt = new Date;
                            console.log("Play called from video: " + tt + tt.getMilliseconds());
                        }
						video.play(); // Start playing after 1st chunk is appended.
						autoPlaybackDone = true;
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
			var arraybuffer;
			var arrayData;
			var fileReader = new FileReader();
			fileReader.onload = function() {
			    arraybuffer = this.result;
				arrayData = new Uint8Array(arraybuffer);

				if(initAudioBuffer == true)
				{
					audioBuffer = arrayData;
					initAudioBuffer = false;
				}
				else
				{
					audioBuffer = _appendBuffer(audioBuffer,arrayData);
				}
				
			    var period = 0;
			    var timeNow = new Date();
			    if(lastAppendTimeAudio != 0)
			    {
			        period = timeNow - lastAppendTimeAudio;
			        //console.log(period);
		        }
		        else
		            lastAppendTimeAudio = timeNow;

				if(period > 100)
				{
					initAudioBuffer = true;
					console.log("Audio source buffer");
					console.log(sourceBuffer);
			        audioSourceBuffer.appendBuffer(audioBuffer);
    		        lastAppendTimeAudio = new Date();
 					if(!autoPlaybackDone)
                        console.log("Appending audio buffer length: " + audioBuffer.byteLength + ", time: " + lastAppendTimeAudio + lastAppendTimeAudio.getMilliseconds());
				}
				
				if (audioSourceBuffer.buffered.length > 0) 
				{
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



