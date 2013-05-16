--TEST--
Check for protocol buffers simple message encoding
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "addressbook.proto.php";

$expect = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "002_simple_encode.bin");

$u = new Tutorial_User();
$u->setId(20);
$u->setOffense(105);
$u->setName("chobie");

$data = ProtocolBuffers::encode($u);
if ($data == $expect) {
    echo "OK" . PHP_EOL;
} else {
    echo $data;
}
--EXPECT--
OK