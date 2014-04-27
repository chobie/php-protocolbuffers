--TEST--
Check for protocol buffers int32 packed implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_int32_packed.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "003_int32_packed_one.bin");

$obj = ProtocolBuffers::decode("Tutorial_Integer32", $bytes);


if ($obj instanceof Tutorial_Integer32) {
    if (count($obj->getValue()) != 1) {
        var_dump($obj);
        exit;
    }
    $values = $obj->getValue();
    if ($values[0] == 1) {
        echo "OK" . PHP_EOL;
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}

ini_set("protocolbuffers.native_scalars", 1);
$obj = ProtocolBuffers::decode("Tutorial_Integer32", $bytes);
if ($obj instanceof Tutorial_Integer32) {
    if (count($obj->getValue()) != 1) {
        var_dump($obj);
        exit;
    }
    $values = $obj->getValue();
    if ($values[0] === 1) {
        echo "OK" . PHP_EOL;
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}

--EXPECT--
OK
OK