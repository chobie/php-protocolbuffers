--TEST--
Check for protocol buffers string implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_string.proto.php";

ini_set("protocolbuffers.validate_string", 0); // do not check utf8 string

$u = new Tutorial_String();
$u->setValue("こんにちは。世界");

try {
    $data = ProtocolBuffers::encode($u);
    echo "OK";
} catch (ProtocolBuffersInvalidByteSequenceException $e) {
    echo "NG";
}
--EXPECT--
OK