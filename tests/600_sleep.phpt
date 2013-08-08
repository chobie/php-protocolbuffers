--TEST--
Check for protocol buffers sleep implementations
--FILE--
<?php

class TestSleep extends ProtocolBuffersMessage
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
            $php = $desc->getOptions()->getExtension("php");
            $php->setUseWakeupAndSleep(true);
            $descriptor = $desc->build();
        }
        return $descriptor;
    }

    public function __sleep()
    {
        echo "Good Night" . PHP_EOL;
        return array();
    }
}

$value = "Hello";
$message = new TestSleep();
$message->setValue($value);

$expected = $message->serializeToString();
$u = ProtocolBuffers::decode("TestSleep", $expected);

--EXPECT--
Good Night