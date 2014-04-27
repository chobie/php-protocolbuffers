--TEST--
Check for protocol buffers sint32 implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_sint32.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "001_sint32_init.bin");

$u = new Tutorial_SInt32();
$u->setValue(0);

$obj = ProtocolBuffers::decode("Tutorial_SInt32", $bytes);

if ($obj instanceof Tutorial_SInt32) {
    if ($obj->getValue() == 0) {
        echo "OK" . PHP_EOL;
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}

ini_set("protocolbuffers.native_scalars", 1);
$obj = ProtocolBuffers::decode("Tutorial_SInt32", $bytes);
if ($obj instanceof Tutorial_SInt32) {
    if ($obj->getValue() === 0) {
        echo "OK" . PHP_EOL;
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}

--EXPECT--
OK
OK