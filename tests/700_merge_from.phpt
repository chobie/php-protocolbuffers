--TEST--
Check for protocol buffers merge from implementations
--FILE--
<?php

class TestMessage extends ProtocolBuffersMessage
{
    protected $double;

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
              "type"     => ProtocolBuffers::TYPE_DOUBLE,
              "name"     => "double",
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

class TestMergeFrom extends ProtocolBuffersMessage
{
    protected $double;
    protected $string;
    protected $long;
    protected $array_int32;
    protected $data;
    protected $message;

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
              "type"     => ProtocolBuffers::TYPE_DOUBLE,
              "name"     => "double",
              "required" => false,
              "optional" => true,
              "repeated" => false,
              "packable" => false,
              "default"  => "",
            )));
            $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_STRING,
              "name"     => "string",
              "required" => false,
              "optional" => true,
              "repeated" => false,
              "packable" => false,
              "default"  => "",
            )));
            $desc->addField(3, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_INT32,
              "name"     => "long",
              "required" => false,
              "optional" => true,
              "repeated" => false,
              "packable" => false,
              "default"  => "",
            )));
            $desc->addField(4, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_INT32,
              "name"     => "array_int32",
              "required" => false,
              "optional" => true,
              "repeated" => true,
              "packable" => false,
              "default"  => "",
            )));
            $desc->addField(5, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_STRING,
              "name"     => "data",
              "required" => false,
              "optional" => true,
              "repeated" => false,
              "packable" => false,
              "default"  => "",
            )));
            $desc->addField(6, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_MESSAGE,
              "name"     => "message",
              "required" => false,
              "optional" => true,
              "repeated" => false,
              "packable" => false,
              "default"  => "",
              "message"  => "TestMessage",
            )));
            $descriptor = $desc->build();
        }
        return $descriptor;
    }
}

$message = new TestMergeFrom();
$message->setDouble(3.14);
$message->setString("Hello");
$message->setLong(128);
$message->appendArrayInt32(128);
$message->appendArrayInt32(256);

$t = new TestMessage();
$t->setDouble(1.28);
$message->setMessage($t);

$output = new TestMergeFrom();
$message->setData("Data");
$message->appendArrayInt32(512);
$output->mergeFrom($message);

echo $output->getDouble() . PHP_EOL;
echo $output->getString() . PHP_EOL;
echo $output->getLong() . PHP_EOL;

$array = $output->getArrayInt32();
asort($array);
echo $array[0] . PHP_EOL;
echo $array[1] . PHP_EOL;
echo $array[2] . PHP_EOL;

echo $output->getData() . PHP_EOL;
$m = $output->getMessage();
echo $m->getDouble() . PHP_EOL;
--EXPECT--
3.14
Hello
128
128
256
512
Data
1.28