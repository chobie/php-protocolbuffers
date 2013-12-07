--TEST--
Check for don't accept null value
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
  $message->setValue(null);
  echo "FAILED" . PHP_EOL;
} catch (Exception $e) {
  echo "OK" . PHP_EOL;
}

--EXPECT--
OK
