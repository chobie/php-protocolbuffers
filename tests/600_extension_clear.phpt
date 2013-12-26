--TEST--
ProtocolBuffersMessage extension implementation
--FILE--
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
            $descriptor = $desc->build();
        }

        return $descriptor;
    }
}
$registry = ProtocolBuffersExtensionRegistry::getInstance();

$registry->add("Person", 256, new ProtocolBuffersFieldDescriptor(array(
    "type"     => ProtocolBuffers::TYPE_INT32,
    "name"     => "address",
    "required" => false,
    "optional" => true,
    "repeated" => false,
    "packable" => false,
    "default"  => null,
)));
$registry->add("Person", 257, new ProtocolBuffersFieldDescriptor(array(
    "type"     => ProtocolBuffers::TYPE_INT32,
    "name"     => "repeated",
    "required" => false,
    "optional" => true,
    "repeated" => true,
    "packable" => false,
    "default"  => null,
)));

$p = new Person();
$p->setExtension("address", 256);
$p->setExtension("repeated", array(1, 2, 3));

$p2 = ProtocolBuffers::decode("Person", $p->serializeToString());
echo $p2->getExtension("address") . PHP_EOL;
foreach ($p2->getExtension("repeated") as $v) {
    echo $v . PHP_EOL;
}
$p2->clearExtension("address");
if (!$p2->hasExtension("address")) {
	echo "OK" . PHP_EOL;
}
$p2->clearExtension("repeated");
if (!$p2->hasExtension("repeated")) {
	echo "OK" . PHP_EOL;
}


--EXPECT--
256
1
2
3
OK
OK