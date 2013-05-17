--TEST--
Check for protocol buffers repeated string implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "repeated_string.proto.php";
$expect = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "002_repeated_string.bin");

$u = new Tutorial_RepeatedString();
$u->addValues("a");
$u->addValues("b");
$u->addValues("c");
$u->addValues("d");
$u->addValues("e");
$u->addValues("f");
$u->addValues("g");
$u->addValues("h");
$u->addValues("i");
$u->addValues("j");

$data = ProtocolBuffers::encode($u);

if ($data == $expect) {
    echo "OK" . PHP_EOL;
} else {
    echo $data;
}
--EXPECT--
OK