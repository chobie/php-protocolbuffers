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
            $desc->addExtensionRange(200, 500);
            $desc->addExtensionRange(600, 700);
            $descriptor = $desc->build();
            $descriptor->dump();
        }

        return $descriptor;
    }
}
$registry = ProtocolBuffersExtensionRegistry::getInstance();

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
$p2 = ProtocolBuffers::decode("Person", $p->serializeToString());

var_dump($p2);
