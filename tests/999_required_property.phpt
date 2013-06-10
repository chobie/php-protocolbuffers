--TEST--
Check for protocol buffers decode implementations
--FILE--
<?php

class Person
{
    protected static $descriptor;

    //protected $_properties = array();

    protected $name;

    public static function getDescriptor()
    {
        if (!isset(self::$descriptor)) {
            $desc = new ProtocolBuffersDescriptorBuilder();
            $desc->addField(1, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_STRING,
                "name"     => "name",
                "packable" => false,
                "repeated" => false,
                "required" => true,
                "default"  => "Hello",
            )));

            self::$descriptor = $desc->build();
        }

        return self::$descriptor;
    }

    public function setName($name)
    {
        $this->name = $name;
    }
}

$person = new Person();

try {
    $data = ProtocolBuffers::encode($person);
    echo "FAILED";
} catch (ProtocolBuffersInvalidProtocolBufferException $e) {
    echo "OK";
}



--EXPECT--
OK