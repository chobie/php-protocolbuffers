--TEST--
Check for toArray implementations
--FILE--
<?php
require "messages" . DIRECTORY_SEPARATOR . "AllTypes.proto.php";

$a = new AllTypes();
$a->set("int32_value", 1);
$a->set("string_value", chr(0x01)); // don't encode wrong value

try {
	$a->toArray();
} catch (ProtocolBuffersInvalidByteSequenceException $e) {
	echo "OK";
}
--EXPECTF--
OK