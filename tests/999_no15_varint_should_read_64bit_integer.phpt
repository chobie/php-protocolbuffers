--TEST--
#15 varint should read 64bit integer
--FILE--
<?php
// https://github.com/chobie/php-protocolbuffers/issues/15

class Time extends ProtocolBuffersMessage
{
  protected $start;

  protected $end;

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
        "type"     => \ProtocolBuffers::TYPE_UINT64,
        "name"     => "start",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => 0,
      )));
      $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_UINT64,
        "name"     => "end",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => 0,
      )));
      $descriptor = $desc->build();
    }

    return $descriptor;
  }
}

$objin = new Time();
$start = time();
$end = $start + 86400;
$objin->setStart($start * 1000);
$objin->setEnd($end * 1000);

$objout = ProtocolBuffers::decode("Time", $objin->serializeToString());

if ($objin->getStart() == $objout->getStart()) {
	echo "OK" . PHP_EOL;
}
if ($objin->getEnd() == $objout->getEnd()) {
	echo "OK" . PHP_EOL;
}

--EXPECT--
OK
OK