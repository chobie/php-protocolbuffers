--TEST--
Check for protocol buffers double packed implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_float_packed.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "003_float_packed.bin");

$u = new Tutorial_Float();
$u->addValue(0);
$u->addValue(1.05);
$u->addValue(3.1418);

$obj = ProtocolBuffers::decode("Tutorial_Float", $bytes);

if ($obj instanceof Tutorial_Float) {
    if (count($obj->getValue()) != 3) {
        var_dump($obj);
        exit;
    }

    $values = $obj->getValue();

    echo $values[0] . PHP_EOL;
    echo $values[1] . PHP_EOL;
    echo $values[2] . PHP_EOL;
} else {
    var_dump($obj);
}


ini_set("protocolbuffers.native_scalars", 1);
$obj = ProtocolBuffers::decode("Tutorial_Float", $bytes);
if ($obj instanceof Tutorial_Float) {
    if (count($obj->getValue()) != 3) {
        var_dump($obj);
        exit;
    }

    $values = $obj->getValue();
    if ($values[0] === 0.0) {
			echo "OK" . PHP_EOL;
    }
} else {
    var_dump($obj);
}


--EXPECT--
0
1.0499999523163
3.1417999267578
OK