--TEST--
Check for JsonSerializable implementations
--SKIPIF--
<?php if (version_compare(PHP_VERSION, "5.4.0") < 0) { die('skip for under 5.4'); } ?>
--FILE--
<?php
require "messages" . DIRECTORY_SEPARATOR . "AllTypes.proto.php";

$params = array(
    "double_value" => "3.0",
    "float_value" => "3.0",
    "int32_value" => "1",
    "int64_value" => "1",
    "uint32_value" => "1",
    "uint64_value" => "1",
    "sint32_value" => "1",
    "sint64_value" => "-1",
    "fixed32_value" => "1",
    "fixed64_value" => "1",
    "sfixed32_value" => "-1",
    "sfixed64_value" => "-1",
    "bool_value" => true,
    "string_value" => "Hello World",
    "bytes_value" => "Hello World",
);

foreach ($params as $key => $value) {
    $a = new AllTypes();
    $a->set($key, $value);
    $result = $a->jsonSerialize();

    if (count($result) == 1 && $result[$key] == $value) {
        echo "OK" . PHP_EOL;
    } else {
        echo "FAULT: {$key}, " . $result[$key] . PHP_EOL;
        var_dump($result);
    }
}

$a = new AllTypes();
foreach ($params as $key => $value) {
    $a->set($key, $value);
}
if ($a->jsonSerialize() == $params) {
    echo "OK" . PHP_EOL;
} else {
    echo "FAULT" . PHP_EOL;
}

--EXPECT--
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK