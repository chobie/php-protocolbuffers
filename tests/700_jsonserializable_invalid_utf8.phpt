--TEST--
Check for JsonSerializable implementations
--SKIPIF--
<?php if (version_compare(PHP_VERSION, "5.4.0") < 0) { die('skip for under 5.4'); } ?>
--FILE--
<?php
require "messages" . DIRECTORY_SEPARATOR . "AllTypes.proto.php";

$a = new AllTypes();
$a->set("int32_value", 1);
$a->set("string_value", chr(0x01)); // don't encode wrong value
$encoded = json_encode($a);

echo $encoded;
--EXPECTF--
Warning: ProtocolBuffersMessage::jsonSerialize(): passed string is not valid utf8 string in %s on line %d
{"int32_value":1}