--TEST--
Check for protocol buffers sfixed32 implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_sfixed32.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "001_sfixed32_signed.bin");

$u = new Tutorial_SFixed32();
$u->setValue(-256);

$obj = ProtocolBuffers::decode("Tutorial_SFixed32", $bytes);

if ($obj instanceof Tutorial_SFixed32) {
    if ($obj->getValue() == -256) {
        echo "OK" . PHP_EOL;
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}

ini_set("protocolbuffers.native_scalars", 1);
$obj = ProtocolBuffers::decode("Tutorial_SFixed32", $bytes);
if ($obj instanceof Tutorial_SFixed32) {
    if ($obj->getValue() === -256) {
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