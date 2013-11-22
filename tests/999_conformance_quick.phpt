--TEST--
Check for protocol buffers decode implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field.proto.php";
$expect = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "999_conformance_quick.bin");

$m = ProtocolBuffers::decode("chobie_github_io_Field", $expect);

echo "double: " . $m->get("v_double") . PHP_EOL;
echo "float: " . $m->get("v_float") . PHP_EOL;
echo "int64: " . $m->get("v_int64") . PHP_EOL;
echo "uint64: " . $m->get("v_uint64") . PHP_EOL;
echo "int32: " . $m->get("v_int32") . PHP_EOL;
echo "fixed64: " . $m->get("v_fixed64") . PHP_EOL;
echo "fixed32: " . $m->get("v_fixed32") . PHP_EOL;
echo "bool: " . $m->get("v_bool") . PHP_EOL;
echo "uint32: " . $m->get("v_uint32") . PHP_EOL;
echo "sfixed32: " . $m->get("v_sfixed32") . PHP_EOL;
echo "sfixed64: " . $m->get("v_sfixed64") . PHP_EOL;
echo "sint32: " . $m->get("v_sint32") . PHP_EOL;
echo "sint64: " . $m->get("v_sint64") . PHP_EOL;
echo "string: " . $m->get("v_string") . PHP_EOL;

echo "int64_min: " . $m->get("v_int64_min") . PHP_EOL;
echo "int32_min: " . $m->get("v_int32_min") . PHP_EOL;
echo "sfixed32_min: " . $m->get("v_sfixed32_min") . PHP_EOL;
echo "sfixed64_min: " . $m->get("v_sfixed64_min") . PHP_EOL;
echo "sint32_min: " . $m->get("v_sint32_min") . PHP_EOL;
echo "sint64_min: " . $m->get("v_sint64_min") . PHP_EOL;
--EXPECT--
double: 3.14
float: 3.14
int64: 9223372036854775807
uint64: 18446744073709551615
int32: 2147483647
fixed64: 18446744073709551615
fixed32: 4294967295
bool: 1
uint32: 4294967295
sfixed32: 2147483647
sfixed64: 9223372036854775807
sint32: 2147483647
sint64: 9223372036854775807
string: Hello World
int64_min: -9223372036854775808
int32_min: -2147483648
sfixed32_min: -2147483648
sfixed64_min: -9223372036854775808
sint32_min: -2147483648
sint64_min: -9223372036854775808