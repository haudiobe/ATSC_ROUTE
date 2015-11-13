<!DOCTYPE html>
<!--
To change this license header, choose License Headers in Project Properties.
To change this template file, choose Tools | Templates
and open the template in the editor.
-->
<html>
    <head>
        <meta charset="UTF-8">
         <script src="../jquery-1.11.1.min.js"></script>
          <link rel="stylesheet" type="text/css" href="SenderUIstyle.css">
        <title></title>
    </head>
    <style>
        h1 {
            position: absolute;
            left: 100px;
            top: 20px;
            color: #A0522D;
        }
       #ip{
           position: absolute;
            left: 100px;
            top: 140px;
       }
       #set{
            position: absolute;
            left: 380px;
            top: 140px;
            width: 55px;
       }
       #initial{
           position: absolute;
            left: 100px;
            top: 220px;
            width: 200px;
       }
    </style>
    <body onload="onloadfunc()">
        <?php
        // put your code here
        ?>
        <h1> Receiver Settings </h1>
        
    <div id="ip">
     IP Address: <input type="text" id="box1" style="width:40px;">.<input type="text" id="box2" style="width:40px;">.
      <input type="text" id="box3"style="width:40px;">.<input type="text" id="box4"style="width:40px;">   
    </div>
  <input type="button" id="set" value="Set" onclick="Set()"><br>
  <input type="button" id="initial" value="Initial Configuration" onclick="Configr()">
  
  
  <script>
      var result;
      function onloadfunc()
      {
		//document.getElementById("myonoffswitch").checked = ('<?php echo intval(file_get_contents("../RcvConfig.txt")); ?>' == 2);	
		Configr();		
          $.ajax({
                      type: 'POST',
                      url: "onloadfunc.php",
                      datatype: "json",
            }).done( function(e) {
              result = JSON.parse(e);
              var spl=result.split(".");
              document.getElementById("box1").value=spl[0];
              document.getElementById("box2").value=spl[1];
              document.getElementById("box3").value=spl[2];
              document.getElementById("box4").value=spl[3];
            });
			

      }
      function Configr()
      {
            $.ajax({
                      type: 'POST',
                      url: "InitConfig.php",
                }).done( function() {
                //console.log("2done");           
                });
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
                
                  });
            }
           
      
  </script>
    </body>
</html>
