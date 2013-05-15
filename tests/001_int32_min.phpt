--TEST--
Check for protocol buffers int32 implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_int32.proto.php";

$expect = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "001_int32_min.bin");

$u = new Tutorial_Integer32();
$u->setValue(-2147483648);

$data = ProtocolBuffers::encode($u);

if ($data == $expect) {
    echo "OK" . PHP_EOL;
} else {
    echo "FAILED" . PHP_EOL;
}
--EXPECT--
OK