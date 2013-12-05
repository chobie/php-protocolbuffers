--TEST--
Regression: multiple packed repeated fields
--FILE--
<?php

class Tutorial_RepeatedInt32 extends ProtocolBuffersMessage
{
  protected $value = array();
  protected $value2 = array();

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
              "packable" => true,
              "repeated" => true,
              "default"  => null,
          )));
          $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_INT32,
              "name"     => "value2",
              "packable" => true,
              "repeated" => true,
              "default"  => null,
          )));

          $descriptor = $desc->build();
      }

      return $descriptor;
  }

}


$u = new Tutorial_RepeatedInt32();
$u->appendValue(1);
$u->appendValue(2);
$u->appendValue(3);
$u->appendValue2(4);
$u->appendValue2(5);
$u->appendValue2(6);

$data = ProtocolBuffers::encode($u);
$r = ProtocolBuffers::decode("Tutorial_RepeatedInt32", $data);
foreach($r->getValue() as $v) {
    echo $v . PHP_EOL;
}
foreach($r->getValue2() as $v) {
    echo $v . PHP_EOL;
}
--EXPECT--
1
2
3
4
5
6
