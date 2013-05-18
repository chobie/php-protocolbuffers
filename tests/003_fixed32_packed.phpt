--TEST--
Check for protocol buffers fixed32 packed implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_fixed32_packed.proto.php";

$expect = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "003_fixed32_packed.bin");

$u = new Tutorial_Fixed32();
$u->addValue(0);
$u->addValue(128);
$u->addValue(256);
$u->addValue(512);
$u->addValue(1024);

$data = ProtocolBuffers::encode($u);

if ($data == $expect) {
    echo "OK" . PHP_EOL;
} else {
    echo $data;
}
--EXPECT--
OK