<?php 
    $data = urldecode($_POST["xml"]);
	
	
	$myFile = "animation.xml";
	$fh = fopen($myFile, 'w') or die("can't open file");
	fwrite($fh, $data);
	fclose($fh);
	
	echo "Saved";
	
?>