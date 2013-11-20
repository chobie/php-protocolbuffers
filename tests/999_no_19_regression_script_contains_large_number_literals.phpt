--TEST--
Script_contains_large_number_literals
--FILE--
<?php
class TestCase extends ProtocolBuffersMessage
{
  protected static $descriptor;
  protected $value;

  public static function getDescriptor()
  {
      if (!isset(self::$descriptor)) {
          $desc = new ProtocolBuffersDescriptorBuilder();
          $desc->addField(1, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_FIXED64,
              "name"     => "value",
              "packable" => false,
              "repeated" => false,
              "default"  => null,
          )));

          self::$descriptor = $desc->build();
      }
      return self::$descriptor;
  }
}

$foo = "1";
$a = new TestCase();
$a->setValue($foo);
echo $a->getValue();
ProtocolBuffers::encode($a);

exit();
// Remove the following line and it works, keep it and it fails
// Yes the line is after the exit() and unrelated, yet it modifies
// the behavior of fixed64 support
$bar = 9223372036854775808;
--EXPECT--
1