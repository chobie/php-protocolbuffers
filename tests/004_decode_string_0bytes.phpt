--TEST--
Check for protocol buffers string implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_string.proto.php";

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "001_string_0bytes.bin");

$u = new Tutorial_String();
$u->setValue("");

$obj = ProtocolBuffers::decode("Tutorial_String", $bytes);

if ($obj instanceof Tutorial_String) {
    if ($obj->getValue() === "") {
        echo "OK";
    } else {
        var_dump($obj);
    }
} else {
    var_dump($obj);
}
--EXPECT--
OK