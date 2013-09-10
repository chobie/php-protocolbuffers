--TEST--
ProtocolBuffersMessage::__construct accepts array parameters
--FILE--
<?php
class Person extends ProtocolBuffersMessage
{
    protected $value;
    protected $float;
    protected $phone;

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
                "required" => false,
                "optional" => false,
                "repeated" => false,
                "packable" => false,
                "default"  => null,
                "message"  => "",
            )));
            $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_FLOAT,
                "name"     => "float",
                "required" => false,
                "optional" => false,
                "repeated" => false,
                "packable" => false,
                "default"  => null,
                "message"  => "",
            )));
            $desc->addField(3, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_MESSAGE,
                "name"     => "phone",
                "required" => false,
                "optional" => false,
                "repeated" => false,
                "packable" => false,
                "default"  => null,
                "message"  => "Phone",
            )));
            $phpoptions = $desc->getOptions()->getExtension(ProtocolBuffers::PHP_MESSAGE_OPTION);
            $descriptor = $desc->build();
        }

        return $descriptor;
    }
}

class Phone extends ProtocolBuffersMessage
{
    protected $value;

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
                "required" => false,
                "optional" => false,
                "repeated" => false,
                "packable" => false,
                "default"  => null,
                "message"  => "",
            )));
            $descriptor = $desc->build();
        }

        return $descriptor;
    }
}

$person = new Person(array(
    "value" => 312,
    "float" => 3.14,
    "phone" => array("value" => 120),
));
echo $person->getValue() . PHP_EOL;
echo $person->getFloat() . PHP_EOL;
$phone = $person->getPhone();

if ($phone instanceof Phone) {
    echo "OK" . PHP_EOL;
} else {
    echo "FAULT" . PHP_EOL;
}
echo $phone->getValue() . PHP_EOL;

--EXPECT--
312
3.14
OK
120