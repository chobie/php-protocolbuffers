--TEST--
Check for protocol buffers fixed32 implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_fixed32.proto.php";

$expect = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "001_fixed32_init.bin");

$u = new Tutorial_Fixed32();
$u->setValue(0);

$data = ProtocolBuffers::encode($u);

if ($data == $expect) {
    echo "OK" . PHP_EOL;
} else {
    echo $data;
}
--EXPECT--
OK