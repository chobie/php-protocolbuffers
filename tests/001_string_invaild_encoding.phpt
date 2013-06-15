--TEST--
Check for protocol buffers string implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_string.proto.php";

$u = new Tutorial_String();
$u->setValue("こんにちは世界");

try {
    $data = ProtocolBuffers::encode($u);
    echo "NG";
} catch (ProtocolBuffersInvalidByteSequenceException $e) {
    echo "OK";
}
--EXPECT--
OK