--TEST--
Check for magic method throws exception
--FILE--
<?php

class Test extends ProtocolBuffersMessage
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
              "type"     => ProtocolBuffers::TYPE_STRING,
              "name"     => "value",
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
try {
  $message->setValue2("HELO"); // check with not exist key.
} catch (Exception $e) {
  echo "OK" . PHP_EOL;
}

--EXPECT--
OK
