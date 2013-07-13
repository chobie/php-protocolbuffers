--TEST--
Check for protocol buffers unknown field bytes implementations
--FILE--
<?php

class TestBytes extends ProtocolBuffersMessage
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
              "type"     => ProtocolBuffers::TYPE_BYTES,
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

$message = new TestBytes();
$message->setValue("Hello World");

$expected = $message->serializeToString();

$u = ProtocolBuffers::decode("Unknown", $expected);
$set = $u->getUnknownFieldSet();
$field = $set->getField(1);

if (current($field->getAsLengthDelimitedList()) === "Hello World") {
	echo "OK" . PHP_EOL;
} else {
	echo "getAsLengthDelimitedList FAILED" . PHP_EOL;
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