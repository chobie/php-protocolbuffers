--TEST--
Check for protocol buffers decode implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_bytes.proto.php";

$droid  = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "android_logo.gif");

try {
	$data = ProtocolBuffers::decode("Tutorial_Bytes", $droid);
} catch (InvalidProtocolBufferException $e) {
	echo "OK";	
}

--EXPECT--
OK