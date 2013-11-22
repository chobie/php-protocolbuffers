--TEST--
Check for protocol buffers unknown field int32 implementations
--FILE--
<?php

class TestInt32 extends ProtocolBuffersMessage
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
              "type"     => ProtocolBuffers::TYPE_INT32,
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

$message = new TestInt32();
$message->setValue(0);

$expected = $message->serializeToString();

$u = ProtocolBuffers::decode("Unknown", $expected);
$set = $u->getUnknownFieldSet();
$field = $set->getField(1);

if ($field->getNumber() == 1) {
	echo "OK" . PHP_EOL;
} else {
	echo "NUMBER FAILED" . PHP_EOL;
}

foreach ($field->getAsVarintList() as $val) {
	if ($val == 0) {
		echo "OK" . PHP_EOL;
	} else {
		echo "getAsVarint FAILED" . PHP_EOL;
	}
}

$bytes = $u->serializeToString();
if ($bytes == $expected) {
	echo "OK" . PHP_EOL;
} else {
	echo "SERIALIZE FAILED" . PHP_EOL;
}


--EXPECT--
OK
OK
OK