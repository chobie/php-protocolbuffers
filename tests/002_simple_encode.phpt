--TEST--
Check for protocol buffers sinple message encoding
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "Base.php";
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "User.php";

$expect = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "002_simple_encode.bin");

$u = new User();
$u->setId(20);
$u->setOffense(105);
$u->setName("chobie");

$data = ProtocolBuffers::encode($u);
if ($data == $expect) {
    echo "OK" . PHP_EOL;
} else {
    echo "FAILED" . PHP_EOL;
}
--EXPECT--
OK