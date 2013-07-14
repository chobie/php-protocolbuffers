--TEST--
Check for protocol buffers unknown field int32 implementations
--FILE--
<?php

class TestFloat extends ProtocolBuffersMessage
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
              "type"     => ProtocolBuffers::TYPE_FLOAT,
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

$message = new TestFloat();
$message->setValue(3.14);

$expected = $message->serializeToString();

$u = ProtocolBuffers::decode("Unknown", $expected);
$set = $u->getUnknownFieldSet();
$field = $set->getField(1);

if ($field->getNumber() === 1) {
	echo "OK" . PHP_EOL;
} else {
	echo "NUMBER FAILED" . PHP_EOL;
}

foreach ($field->getAsLengthDelimitedList() as $val) {
    $val = current(unpack("f", $val));
    if (round($val, 2) == 3.14) {
        echo "OK" . PHP_EOL;
    } else {
        echo "Failed: " . var_export($val);
    }
}

foreach ($field->getAsFloatList() as $val) {
    if (round($val, 2) == 3.14) {
        echo "OK" . PHP_EOL;
    } else {
        echo "Failed: " . var_export($val);
    }
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