--TEST--
Check for protocol buffers decoding bytes bug
--FILE--
<?php

class Bytes extends ProtocolBuffersMessage
{
  protected $key;

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
        "type"     => ProtocolBuffers::TYPE_BYTES,
        "name"     => "key",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => "",
      )));
      $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_BYTES,
        "name"     => "value",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => "",
      )));
      $descriptor = $desc->build();
    }

    return $descriptor;
  }
}

$bytes = new Bytes();
$bytes->setKey("hello");
$bytes->setValue("world");

$data = $bytes->serializeToString();
$bytes = ProtocolBuffers::decode("Bytes", $data);

echo $bytes->getKey() . PHP_EOL;
echo $bytes->getValue() . PHP_EOL;

--EXPECT--
hello
world