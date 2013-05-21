--TEST--
Check for protocol buffers sint32 packed implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_sint32_packed.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "003_sint32_packed.bin");

$u = new Tutorial_SignedInteger32();
$u->addValue(-1024);
$u->addValue(0);
$u->addValue(1024);

$obj = ProtocolBuffers::decode("Tutorial_SignedInteger32", $bytes);

if ($obj instanceof Tutorial_SignedInteger32) {
    if (count($obj->getValue()) != 3) {
        var_dump($obj);
        exit;
    }
    $values = $obj->getValue();
    if ($values[0] === -1024 && $values[1] === 0 && $values[2] === 1024) {
        echo "OK";
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}


--EXPECT--
OK