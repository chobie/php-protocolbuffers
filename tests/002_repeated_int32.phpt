--TEST--
Check for protocol buffers repeated int32 implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "repeated_int32.proto.php";
$expect = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "002_repeated_int32.bin");

$u = new Tutorial_RepeatedInt32();
$u->addValue(1);
$u->addValue(2);
$u->addValue(3);
$u->addValue(4);
$u->addValue(5);
$u->addValue(6);
$u->addValue(7);
$u->addValue(8);
$u->addValue(9);
$u->addValue(10);

$data = ProtocolBuffers::encode($u);

if ($data == $expect) {
    echo "OK" . PHP_EOL;
} else {
    echo $data;
}
--EXPECT--
OK