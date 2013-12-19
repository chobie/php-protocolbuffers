--TEST--
Check for protocol buffers set from implementations
--FILE--
<?php

class Phone extends ProtocolBuffersMessage
{
    protected $number;

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
              "name"     => "number",
              "required" => false,
              "optional" => true,
              "repeated" => false,
              "packable" => false,
              "default"  => "",
            )));
            $descriptor = $desc->build();
        }
        return $descriptor;
    }
}

class Person extends ProtocolBuffersMessage
{
    protected $name;
    protected $age;
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
              "type"     => ProtocolBuffers::TYPE_STRING,
              "name"     => "name",
              "required" => false,
              "optional" => true,
              "repeated" => false,
              "packable" => false,
              "default"  => "",
            )));
            $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_INT32,
              "name"     => "age",
              "required" => false,
              "optional" => true,
              "repeated" => false,
              "packable" => false,
              "default"  => "",
            )));
            $desc->addField(3, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_MESSAGE,
              "name"     => "phone",
              "required" => false,
              "optional" => true,
              "repeated" => false,
              "packable" => false,
              "default"  => "",
              "message" => "Phone",
            )));
            $descriptor = $desc->build();
        }
        return $descriptor;
    }
}

$message = new Person();
$message->setFrom(array(
	"name" => "John",
	"age"  => 32,
));

echo $message->getName() . PHP_EOL;
echo $message->getAge() . PHP_EOL;

$message->setFrom(array(
	"name" => "Akira",
	"age"  => 24,
));

echo $message->getName() . PHP_EOL;
echo $message->getAge() . PHP_EOL;


$message->setFrom(array(
    "name" => "Ken",
    "age"  => 24,
    "phone" => new Phone(array("number" => "12345")),
));

echo $message->getName() . PHP_EOL;
echo $message->getAge() . PHP_EOL;
$phone = $message->getPhone();
echo $phone->getNumber() . PHP_EOL;

--EXPECT--
John
32
Akira
24
Ken
24
12345