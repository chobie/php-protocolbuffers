<?php
class Person extends ProtocolBuffersMessage
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
                "message"  => "Tutorial_Person",
            )));
            $descriptor = $desc->build();
        }

        return $descriptor;
    }
}

$registry = ProtocolBuffers\ExtensionRegistry::getInstance();

// You can extend Person message like this. (normally we generate these code from .proto. adding by hand is bad idea.)
$registry->add("Person", 256, new ProtocolBuffersFieldDescriptor(array(
    "type"     => ProtocolBuffers::TYPE_INT32,
    "name"     => "address",
    "required" => false,
    "optional" => false,
    "repeated" => false,
    "packable" => false,
    "default"  => null,
)));

$p = new Person();
$p->setExtension("address", 256);
$p2 = Person::parseFromString($p->serializeToString());

var_dump($p2);
