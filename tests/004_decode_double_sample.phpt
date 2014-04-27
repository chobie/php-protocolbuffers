--TEST--
Check for protocol buffers double implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_double.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "001_double_sample.bin");

$obj = ProtocolBuffers::decode("Tutorial_Double", $bytes);

if ($obj instanceof Tutorial_Double) {
    if (round($obj->getValue(), 2) == 1.05) {
        echo "OK" . PHP_EOL;
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}

ini_set("protocolbuffers.native_scalars", 1);
if ($obj instanceof Tutorial_Double) {
    if (round($obj->getValue(), 2) === 1.05) {
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