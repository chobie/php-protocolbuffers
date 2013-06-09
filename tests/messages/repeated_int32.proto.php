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

  public function getValue()
  {
    $result = null;

    if (array_key_exists('value', $this->_properties)) {
      $result = $this->_properties['value'];
    }

    return $result;
  }

  public function addValue($value)
  {
    $this->_properties['value'][] = $value;
  }

  /**
   * get descriptor for protocol buffers
   * 
   * @return array
   */
  public static function getDescriptor()
  {
      if (!isset(self::$descriptor)) {
          $desc = new ProtocolBuffers_DescriptorBuilder();
          $desc->addField(1, new ProtocolBuffers_FieldDescriptor(array(
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

