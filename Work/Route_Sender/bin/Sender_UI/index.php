<!DOCTYPE html>
<!--
To change this license header, choose License Headers in Project Properties.
To change this template file, choose Tools | Templates
and open the template in the editor.
-->
<html>
    <head>
        <meta charset="UTF-8">
        <title>Route Sender</title>
        <script src="jquery-1.11.1.min.js"></script>
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
        <span id="percentage"></span>
   
        <div class="onoffswitch">
            <input onchange="checkthis(this)" type="checkbox" name="onoffswitch" class="onoffswitch-checkbox" id="myonoffswitch" unchecked>
            <label class="onoffswitch-label" for="myonoffswitch">
                <span class="onoffswitch-inner"></span>
                <span class="onoffswitch-switch"></span>
            </label>
        </div>
        
    </head>
    <body onload="onloadfunction()"onbeforeunload="return offfunction()">

        <h1>ROUTE Sender</h1>
        <!--input type=button id="On" value="ON" onclick="Onfunction()">  
        <input type=button id="Off" value="OFF" onclick="Offfunction()"-->  
        <div id="ip">
            IP Address: <select type="text" id="ipselect" onchange="Set()" />  <input type="hidden" id="box4"style="width:4px;">
            <input type="checkbox" id="trafficFilter" onchange="applyFilter()" unchecked> Apply Traffic Filter<br>
        </div>

        <input type="button" id="initial" value="Initial Configuration" onclick="Configr()">
        <div id="Ad">
            Ad Time:<input type="number" id="AdTime" min="0" max="800" style="width:80px;" onchange="saveAdTime()"> sec
        </div>
        <!--p id="status">Status</p-->
        <script>
            var flag=0;
            var es;
            var result;
            var totalTime = 900;
            var timeOffset = 3;
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
                    var ipselect = document.getElementById('ipselect');

                    if(ipselect.options.length === 0)
                    {
                        for(var index = 0; index < result.length ; index ++)
                        {
                            ipselect.options[ipselect.options.length] = new Option(result[index], result[index]);
                        }
                    }
                    Set();
                    if(restart)
                        Onfunction();
                    restart = false;
                });
                
                applyFilter();
                loadAdTime();
            }
            
            function applyFilter()
            {
                if (document.getElementById('trafficFilter').checked) {
                    //do traffic filtering only when the checkbox is checked
                    var ipselect = document.getElementById('ipselect');
                    if(ipselect.options.length <= 0)
                    {
                        console.log("WARNING: IP not set!\n");
                        return;
                    }
                    var ipstr = ipselect.options[ipselect.selectedIndex].value;
                    var netname = ipstr.split(" ")[1].slice(1,-1);
                    console.log(netname);
                    $.ajax({
                        type: 'POST',
                        url: "trafficFilter.php",
                        datatype: "json",
                        data: {netname: JSON.stringify(netname)},
                    }).done( function(e) {
                       console.log("Ran trafficFilter.php, result: " + e);
                    });
                }
            }
            
            function checkthis(ele)
            {
                if(ele.checked)
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
                progressCallback = setInterval(function(){progressFunction(); }, 1000);
            }
          
            function progressFunction() {
                timeProgress = timeProgress + 1;
                var pBar = document.getElementById('progressor');
                var adjustedTime = Math.max(timeProgress - timeOffset , 0);
                pBar.value = adjustedTime/(totalTime)*100;
                var perc = document.getElementById('percentage');
                //perc.innerHTML   = Math.round(adjustedTime/totalTime*100) + "%";
                var progSec = (adjustedTime - 60*Math.floor(adjustedTime/60));    //('  ' + progSec).substr(-2)
                perc.innerHTML = Math.floor(adjustedTime/60) + " min " + progSec + " sec / " + Math.floor(totalTime/60) + " min " + (totalTime - 60*Math.floor(totalTime/60)) + " sec"; 
                document.getElementById('AdTime').disabled = true;

                if(adjustedTime >= totalTime)
                {
                    restart = true;
                    Offfunction();
                }
            };
       
            function Offfunction()
            {
                flag=1;
                //onloadfunction();
                var pBar = document.getElementById('progressor');
                pBar.value = 0;
                var perc = document.getElementById('percentage');
                perc.innerHTML   = "";
                document.getElementById('AdTime').disabled = false;
                clearInterval(progressCallback);
                timeProgress = 0;
                $.ajax({
                    //type: 'POST',
                    url: "onloadaction.php",
                    datatype: "json",
                }).done( function(e) {
                    console.log("Completed Off job.");
                    if(restart == true)
                    {
                        Onfunction();
                        console.log("Asked restart.");
                        restart = false;
                    }
                });
            }
            
            function Set()
            {
                var ipselect = document.getElementById('ipselect');
                
                if(ipselect.options.length <= 0)
                    return;
                
                var ipstr = ipselect.options[ipselect.selectedIndex].value;
                var ipaddr = ipstr.split(" ")[0];
                console.log(ipaddr);
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
//                    console.log("3done");
                })
            }
            
            function loadAdTime()
            {
                $.ajax({
                    type: 'POST',
                    url: "loadAdTime.php",
                    datatype: 'json'
                }).done( function(AdTime) {
                    document.getElementById('AdTime').value = new Number(AdTime);
                    console.log(AdTime);
//                    console.log("4done");
                })
            }
        </script>
    </body>
</html>
