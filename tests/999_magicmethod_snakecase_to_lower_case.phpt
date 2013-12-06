--TEST--
Check for snake case to underscore conversion
--FILE--
<?php

class Test extends ProtocolBuffersMessage
{
    protected $DEPRECATEDSentence;

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
              "name"     => "DEPRECATEDSentence",
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

$message = new Test();
$value = "Hello";
$message->setDEPRECATEDSentence($value);
echo $message->getDEPRECATEDSentence();

--EXPECT--
Hello
