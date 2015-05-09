--TEST--
Check for protocol buffers int32 implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_int32.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "001_int32_max.bin");

$u = new Tutorial_Integer32();
$u->setValue(2147483647);

$obj = ProtocolBuffers::decode("Tutorial_Integer32", $bytes);

if ($obj instanceof Tutorial_Integer32) {
    if ($obj->getValue() == 2147483647) {
        echo "OK";
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}
--EXPECT--
OK