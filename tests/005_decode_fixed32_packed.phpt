--TEST--
Check for protocol buffers fixed32 packed implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_fixed32_packed.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "003_fixed32_packed.bin");

$u = new Tutorial_Fixed32();
$u->addValue(0);
$u->addValue(128);
$u->addValue(256);
$u->addValue(512);
$u->addValue(1024);

$obj = ProtocolBuffers::decode("Tutorial_Fixed32", $bytes);

if ($obj instanceof Tutorial_Fixed32) {
    if (count($obj->getValue()) != 5) {
        var_dump($obj);
        exit;
    }
    $values = $obj->getValue();
    if ($values[0] == 0 && $values[1] == 128 && $values[2] == 256 && $values[3] == 512 && $values[4] == 1024) {
        echo "OK" . PHP_EOL;
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}

ini_set("protocolbuffers.native_scalars", 1);
$obj = ProtocolBuffers::decode("Tutorial_Fixed32", $bytes);
if ($obj instanceof Tutorial_Fixed32) {
    if (count($obj->getValue()) != 5) {
        var_dump($obj);
        exit;
    }
    $values = $obj->getValue();
    if ($values[0] === 0 && $values[1] === 128 && $values[2] === 256 && $values[3] === 512 && $values[4] === 1024) {
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