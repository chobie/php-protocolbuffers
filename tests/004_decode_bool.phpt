--TEST--
Check for protocol buffers bool implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_bool.proto.php";

$data = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "001_bool.bin");

$u = new Tutorial_Boolean();
$u->setValue(true);

$obj = ProtocolBuffers::decode("Tutorial_Boolean", $data);

if ($obj instanceof Tutorial_Boolean) {
    if ($obj->getValue() === true) {
        echo "OK" . PHP_EOL;
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}
--EXPECT--
OK