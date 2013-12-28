--TEST--
Check for protocol buffers string implementations
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "field_string.proto.php";

$expect = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "001_string_0bytes.bin");

class MyString
{
	protected $buffer = "";

	public function append($string)
	{
		$this->buffer .= $string;
	}

	public function __toString()
	{
		return $this->buffer;
	}
}

$u = new Tutorial_String();

$string = new MyString();
$string->append("Hello");
$u->setValue($string);

$u2 = ProtocolBuffers::decode("Tutorial_String", ProtocolBuffers::encode($u));

echo $u2->getValue() . PHP_EOL;
if (is_string($u2->getValue())) {
	echo "OK" . PHP_EOL;
} else {
	echo "FAULT" . PHP_EOL;
}
if ($u->getValue() instanceof MyString) {
	echo "OK" . PHP_EOL;
} else {
	echo "FAULT" . PHP_EOL;
}

--EXPECT--
Hello
OK
OK