<!DOCTYPE html>
<!--
To change this license header, choose License Headers in Project Properties.
To change this template file, choose Tools | Templates
and open the template in the editor.
-->
<html>
    <head>
        <meta charset="UTF-8">
        <title></title>
        <script src="//ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
        <script src="//ajax.googleapis.com/ajax/libs/jquery/1.11.0/jquery.min.js"></script> 
        <link rel="stylesheet" type="text/css" href="SenderUIstyle.css">
        <style>
        h1 {
            position: absolute;
            left: 100px;
            top: 20px;
            color: #009900;
        }
        /*#On{
            position: absolute;
            left: 100px;
            top: 100px;
            width:100px;
            color: #009900;
        }*/
        #status{
            position: absolute;
            left: 100px;
            top: 450px;
        }
        #progressor{
             position: absolute;
            left: 100px;
            top: 150px;
            width: 500px;
            height: 25px;
        }
        #percentage{
             position: absolute;
            left: 100px;
            top: 190px;
        }
        
        #ip{
              position: absolute;
          left: 100px;
            top: 230px;
            
        }
        #initial{
            position: absolute;
          left: 100px;
            top: 340px;
            width: 200px;
        }
        #set{
             position: absolute;
          left: 380px;
            top: 230px;
            width: 70px;
        }
        .onoffswitch {
            position: absolute; width: 100px;left: 100px;top: 90px;
           -webkit-user-select:none; -moz-user-select:none; -ms-user-select: none;
       }
       #Ad{
             position: absolute;
             left: 100px;
            top: 280px;
           
        }
            </style>
     <progress id='progressor' value="0" max='100'></progress>  
        <span id="percentage">Percentage %</span>
   
        <div class="onoffswitch">
            <input onchange="checkthis(this)" type="checkbox" name="onoffswitch" class="onoffswitch-checkbox" id="myonoffswitch" unchecked>
        <label class="onoffswitch-label" for="myonoffswitch">
        <span class="onoffswitch-inner"></span>
        <span class="onoffswitch-switch"></span>
       </label>
      </div>
        
    </head>
    <body onload="onloadfunction()"onbeforeunload="return offfunction()">
        <?php
        // put your code here
        ?>
        <h1>Route Sender</h1>
  <!--input type=button id="On" value="ON" onclick="Onfunction()">  
  <input type=button id="Off" value="OFF" onclick="Offfunction()"-->  
  <div id="ip">
     IP Address: <input type="text" id="box1" style="width:40px;">.<input type="text" id="box2" style="width:40px;">.
      <input type="text" id="box3"style="width:40px;">.<input type="text" id="box4"style="width:40px;">   
  </div>
  <input type="button" id="set" value="Set" onclick="Set()"><br>
  <input type="button" id="initial" value="Initial Configuration" onclick="Configr()">
  <div id="Ad">
      Ad Time  :  <input type="number" id="AdTime" value="30" min="0" max="500" style="width:80px;" onchange="saveAdTime()">
  </div>
  <!--p id="status">Status</p-->
        <script>
            var flag=0;
              var es;
              var result;
			  var totalTime = 750;
			  var timeProgress = 0;
			  var progressCallback;
			  var restart = false;
			  
           function onloadfunction()
           {
               $.ajax({
                      //type: 'POST',
                      url: "onloadaction.php",
                      datatype: "json",
            }).done( function(e) {
                 //alert("Killed all"); 
              result = JSON.parse(e);
              var spl=result.split(".");
              document.getElementById("box1").value=spl[0];
              document.getElementById("box2").value=spl[1];
              document.getElementById("box3").value=spl[2];
              document.getElementById("box4").value=spl[3];
			  if(restart)
				  Onfunction();
			  restart = false;
            })
            Set();
			saveAdTime();
           }
		   
          function checkthis(ele){
              
           if(ele.checked )
           {
              Onfunction();
          }
          else
          {
              Offfunction();
          }
             }
    
           function Onfunction()
            { 
                $.ajax({
                      type: 'POST',
                      url: "sender.php",
                      data: {}
            }).done( function() {
           
            }).fail(function() {
         
            });
			progressCallback = setInterval(function(){ progressFunction(); }, 1000);
          }
		  
		    function progressFunction() {
				timeProgress = timeProgress + 1;
            var pBar = document.getElementById('progressor');
            pBar.value = timeProgress/(totalTime)*100;
            var perc = document.getElementById('percentage');
            perc.innerHTML   = Math.round(timeProgress/totalTime*100) + "%";
			document.getElementById('AdTime').disabled = true;	
			
			if(timeProgress >= totalTime)
			{
				restart = true;
				Offfunction();
			}
                        
            };
       
          function Offfunction()
            {
                flag=1;
                onloadfunction();
				var pBar = document.getElementById('progressor');
				pBar.value = 0;
				var perc = document.getElementById('percentage');
				perc.innerHTML   = "0%";
				document.getElementById('AdTime').disabled = false;	
				clearInterval(progressCallback);
				timeProgress = 0;				
            }
            
            function Set()
            { 
                val1=document.getElementById("box1").value;
                val2=document.getElementById("box2").value;
                val3=document.getElementById("box3").value;
                val4=document.getElementById("box4").value;
                ipaddr=val1+"."+val2+"."+val3+"."+val4;
                $.ajax({
                      type: 'POST',
                      url: "setIP.php",
                       datatype: "json",
                      data: {ip: JSON.stringify(ipaddr)},
            }).done( function() {
               // console.log("1done");
                
            })
            }
            function Configr()
            {
                $.ajax({
                      type: 'POST',
                      url: "InitConfig.php",
                }).done( function() {
                //console.log("2done");
                
                })
            
            }
            function saveAdTime()
            {
                time=document.getElementById("AdTime").value;
                console.log(time);
                $.ajax({
                      type: 'POST',
                      url: "saveAdTime.php",
                      datatype: 'json',
                      data : {time:JSON.stringify(time)}
                }).done( function() {
                console.log("3done");
                
                })
            }
            
           
        </script>
    </body>
</html>
