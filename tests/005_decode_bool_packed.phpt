--TEST--
Check for protocol buffers bool packed implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_bool_packed.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "003_bool_packed.bin");

$u = new Tutorial_Boolean();
$u->addValue(1);
$u->addValue(0);

$obj = ProtocolBuffers::decode("Tutorial_Boolean", $bytes);

if ($obj instanceof Tutorial_Boolean) {
    if (count($obj->getValue()) != 2) {
        var_dump($obj);
        exit;
    }
    $values = $obj->getValue();
    if ($values[0] === true && $values[1] === false) {
        echo "OK" . PHP_EOL;
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}

ini_set("protocolbuffers.native_scalars", 1);
$obj = ProtocolBuffers::decode("Tutorial_Boolean", $bytes);
if ($obj instanceof Tutorial_Boolean) {
    if (count($obj->getValue()) != 2) {
        var_dump($obj);
        exit;
    }
    $values = $obj->getValue();
    if ($values[0] === true && $values[1] === false) {
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