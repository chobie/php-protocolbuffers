--TEST--
check default value
--FILE--
<?php
class DefaultValue extends ProtocolBuffersMessage
{
  protected $start;

  protected $end;

  protected $greeting;

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
        "type"     => ProtocolBuffers::TYPE_UINT64,
        "name"     => "start",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => 124,
      )));
      $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_UINT64,
        "name"     => "end",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => 312,
      )));
      $desc->addField(3, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_STRING,
        "name"     => "greeting",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => "Hello World",
      )));
      $descriptor = $desc->build();
    }

    return $descriptor;
  }
}

$obj = new DefaultValue();

echo $obj->getStart() . PHP_EOL;
echo $obj->getEnd() . PHP_EOL;
echo $obj->getGreeting() . PHP_EOL;

--EXPECT--
124
312
Hello World
