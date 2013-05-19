--TEST--
Check for protocol buffers sint32 implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_sint32.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "001_sint32_min.bin");

$u = new Tutorial_SInt32();
$u->setValue(-2147483648);

$obj = ProtocolBuffers::decode("Tutorial_SInt32", $bytes);

if ($obj instanceof Tutorial_SInt32) {
    if ($obj->getValue() == -2147483648) {
        echo "OK";
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}
--EXPECT--
OK