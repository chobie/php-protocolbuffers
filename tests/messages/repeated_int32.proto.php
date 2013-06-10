<?php
/**
 * Tutorial_RepeatedInt32
 *
 * @message Tutorial.RepeatedInt32
 *
 * -*- magic properties -*-
 *
 * @property array $value
 */
class Tutorial_RepeatedInt32
{
  protected static $descriptor;

  protected $_properties = array();

  protected $value = array();

  public function getValue()
  {
    return $this->value;
  }

  public function addValue($value)
  {
    $this->value[] = $value;
  }

  /**
   * get descriptor for protocol buffers
   * 
   * @return array
   */
  public static function getDescriptor()
  {
      if (!isset(self::$descriptor)) {
          $desc = new ProtocolBuffersDescriptorBuilder();
          $desc->addField(1, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_INT32,
              "name"     => "value",
              "packable" => false,
              "repeated" => true,
              "default"  => null,
          )));

          self::$descriptor = $desc->build();
      }

      return self::$descriptor;
  }

}

