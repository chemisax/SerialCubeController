<?php 
    $myFile = "animation.xml";
	$fh = fopen($myFile, 'r');
	$theData = fread($fh, filesize($myFile));
	fclose($fh);
	echo $theData;
?>