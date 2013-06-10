<?php
/**
 * Tutorial_Float
 *
 * @message Tutorial.Float
 *
 * -*- magic properties -*-
 *
 * @property array $value
 */
class Tutorial_Float
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
              "type"     => ProtocolBuffers::TYPE_FLOAT,
              "name"     => "value",
              "packable" => true,
              "repeated" => true,
              "default"  => null,
          )));

          self::$descriptor = $desc->build();
      }

      return self::$descriptor;

  }

}

