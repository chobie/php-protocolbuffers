--TEST--
Check for protocol buffers bytes implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_bytes.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "001_bytes.bin");
$droid  = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "android_logo.gif");

$obj = ProtocolBuffers::decode("Tutorial_Bytes", $bytes);

if ($obj instanceof Tutorial_Bytes) {
    if ($obj->getValue() === $droid) {
        echo "OK" . PHP_EOL;
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}

ini_set("protocolbuffers.native_scalars", 1);
$obj = ProtocolBuffers::decode("Tutorial_Bytes", $bytes);

if ($obj instanceof Tutorial_Bytes) {
    if ($obj->getValue() === $droid) {
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