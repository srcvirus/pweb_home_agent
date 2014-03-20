<?php
	$url = "http://localhost:8080/?";
	$first_token = true;
	foreach ($_GET as $key => $value)
	{
		if(!$first_token) $url .= "&";
		$url .= "$key"."="."$value";
		$first_token = false;
	}

	foreach($_PUT as $key => $value)
	{
		if(!$first_token) $url .= "&";
		$url .= "$key"."="."$value";
		$first_token = false;
	}

	$response = file_get_contents($url);
	$respLength = strlen($response);
	header("Content-Length: $respLength");
	header("Content-type: text/html");
	header("Access-Control-Allow-Origin: *");
	echo $response;
?>
