--TEST--
Check for protocol buffers string implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_string.proto.php";

$u = new Tutorial_String();
$u->setValue("ëfê∞ÇÁÇµÇ¢ê¢äE");

try {
    $data = ProtocolBuffers::encode($u);
    echo "NG";
} catch (ProtocolBuffers_InvalidByteSequenceException $e) {
    echo "OK";
}
--EXPECT--
OK