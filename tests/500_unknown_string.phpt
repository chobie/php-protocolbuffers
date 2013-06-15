--TEST--
Check for protocol buffers unknown field string implementations
--FILE--
<?php

class TestString extends ProtocolBuffersMessage
{
    protected $value;

	public function setValue($value)
	{
		$this->value = $value;
	}
    /**
     * get descriptor for protocol buffers
     *
     * @return array
     */
    public static function getDescriptor()
    {
        static $descriptor;
        if (!isset($descriptor)) {
            $desc = new ProtocolBuffersDescriptorBuilder();
            $desc->addField(1, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_STRING,
              "name"     => "value",
              "required" => true,
              "optional" => false,
              "repeated" => false,
              "packable" => false,
              "default"  => "",
            )));
            $descriptor = $desc->build();
        }
        return $descriptor;
    }
}

class Unknown extends ProtocolBuffersMessage
{
    protected $value;
    protected $_unknown;

    public function getUnknownFieldSet()
    {
        return $this->_unknown;
    }

    /**
     * get descriptor for protocol buffers
     *
     * @return array
     */
    public static function getDescriptor()
    {
        static $descriptor;
        if (!isset($descriptor)) {
            $desc = new ProtocolBuffersDescriptorBuilder();
            $php = $desc->getOptions()->getExtension("php");
            $php->setProcessUnknownFields(true);
            $descriptor = $desc->build();
        }
        return $descriptor;
    }
}

$value = "Hello";
$message = new TestString();
$message->setValue($value);

$expected = $message->serializeToString();

$u = ProtocolBuffers::decode("Unknown", $expected);
$set = $u->getUnknownFieldSet();
$field = $set->getField(1);

if ($field->getNumber() === 1) {
	echo "OK" . PHP_EOL;
} else {
	echo "NUMBER FAILED" . PHP_EOL;
}

if ($field->getAsLengthDelimited() === $value) {
	echo "OK" . PHP_EOL;
} else {
	echo "getAsLengthDelimited FAILED" . PHP_EOL;
}

if ($field->getLengthDelimitedSize() === strlen($value)) {
	echo "OK" . PHP_EOL;
} else {
	echo "getLengthDelimitedSize FAILED" . PHP_EOL;
}

$bytes = $u->serializeToString();
if ($bytes === $expected) {
	echo "OK" . PHP_EOL;
} else {
	echo "SERIALIZE FAILED" . PHP_EOL;
}

--EXPECT--
OK
OK
OK
OK