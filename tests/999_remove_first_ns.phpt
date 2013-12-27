--TEST--
Check for remove first namespace separator
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_fixed32.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "001_fixed32_init.bin");

// should be ok
$obj = ProtocolBuffers::decode("\\Tutorial_Fixed32", $bytes);

if ($obj instanceof Tutorial_Fixed32) {
    if ($obj->getValue() == 0) {
        echo "OK";
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}
--EXPECT--
OK