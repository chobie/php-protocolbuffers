--TEST--
Check for protocol buffers sfixed64 packed implementations
--SKIPIF--
<?php if (8 !== PHP_INT_SIZE) { die('skip Only for 64-bit platform'); } ?>
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_sfixed64_packed.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "003_sfixed64_packed.bin");

$u = new Tutorial_SFixed64();
$u->addValue(-1024);
$u->addValue(0);
$u->addValue(1024);

$obj = ProtocolBuffers::decode("Tutorial_SFixed64", $bytes);

if ($obj instanceof Tutorial_SFixed64) {
    if (count($obj->getValue()) != 3) {
        var_dump($obj);
        exit;
    }
    $values = $obj->getValue();

    if ($values[0] == -1024 && $values[1] == 0 && $values[2] == 1024) {
        echo "OK";
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}


--EXPECT--
OK