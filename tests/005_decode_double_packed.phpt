--TEST--
Check for protocol buffers double packed implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_double_packed.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "003_double_packed.bin");

$u = new Tutorial_Double();
$u->addValue(0);
$u->addValue(1.05);
$u->addValue(3.1418);

$obj = ProtocolBuffers::decode("Tutorial_Double", $bytes);

if ($obj instanceof Tutorial_Double) {
    if (count($obj->getValue()) != 3) {
        var_dump($obj);
        exit;
    }
    $values = $obj->getValue();
    if ($values[0] == 0 && $values[1] == 1.05 && $values[2] == 3.1418) {
        echo "OK";
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}

--EXPECT--
OK