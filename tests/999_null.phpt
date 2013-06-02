--TEST--
Check for protocol buffers decode implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_bytes.proto.php";

try {
	$data = ProtocolBuffers::decode("Tutorial_Bytes", NULL);
} catch (InvalidProtocolBufferException $e) {
	echo "OK";	
}

--EXPECT--
OK