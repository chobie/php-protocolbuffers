--TEST--
Check for protocol buffers int32 packed implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_int32_packed.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "003_int32_packed_many.bin");

$obj = ProtocolBuffers::decode("Tutorial_Integer32", $bytes);

if ($obj instanceof Tutorial_Integer32) {
    if (count($obj->getValue()) != 3) {
        var_dump($obj);
        exit;
    }

    $values = $obj->getValue();

    echo $values[0] . PHP_EOL;
    echo $values[1] . PHP_EOL;
    echo $values[2] . PHP_EOL;
} else {
    var_dump($obj);
}

--EXPECT--
1
2
1024