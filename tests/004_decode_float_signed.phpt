--TEST--
Check for protocol buffers float implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_float.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "001_float_signed.bin");

$u = new Tutorial_Float();
$u->setValue(-1.05);

$obj = ProtocolBuffers::decode("Tutorial_Float", $bytes);

if ($obj instanceof Tutorial_Float) {
    if (round($obj->getValue(), 2) == -1.05) {
        echo "OK" . PHP_EOL;
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}

ini_set("protocolbuffers.native_scalars", 1);
$obj = ProtocolBuffers::decode("Tutorial_Float", $bytes);
if ($obj instanceof Tutorial_Float) {
    if (round($obj->getValue(), 2) === -1.05) {
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