<!DOCTYPE html>
<html>
<head>
  <title>Test</title>
</head>
<body>
  Running test
</body>
</html>
<script src="../jquery-1.11.1.min.js"></script>
<script type="text/javascript">

window.onload = function (e) {
    $.post(
            "Testteardown.php",
            {channel:"1"},
            function(response)
            {
				$.post(
						"StartTest.php",
						{channel:"1"},
						function(response)
						{
							console.log(response);
						}
					   );
					   
					   setTimeout(function () {window.open("http://192.168.121.130/Work/Route_Receiver/Receiver/")}, 15000);
					   //setTimeout(function () {testTeardown()}, 25000);
            }
           );
		   
};

function testTeardown(){

    $.post(
            "Testteardown.php",
            {channel:"1"},
            function(response)
            {
                console.log(response);
            }
           );
		   
};

</script>