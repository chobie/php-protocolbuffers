--TEST--
Check for protocol buffers sleep implementations
--FILE--
<?php

class TestSleep extends ProtocolBuffersMessage
{
    protected $value;

    protected $name;

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
              "required" => false,
              "optional" => true,
              "repeated" => false,
              "packable" => false,
              "default"  => "",
            )));
            $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_STRING,
                "name"     => "name",
                "required" => false,
                "optional" => true,
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
        return array("value");
    }
}

$value = "Hello";
$value2 = "World";
$message = new TestSleep();
$message->setValue($value);
$message->setName($value2);

$expected = $message->serializeToString();

$u = ProtocolBuffers::decode("TestSleep", $expected);

echo $u->getValue();
echo $u->getName();
--EXPECT--
Hello
