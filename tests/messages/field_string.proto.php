<?php
/**
 * Tutorial_String
 *
 * @message Tutorial.String
 *
 * -*- magic properties -*-
 *
 * @property string $value
 */
class Tutorial_String
{
  protected static $descriptor;

  protected $_properties = array();

  protected $value;

  public function getValue()
  {
    return $this->value;
  }

  public function setValue($value)
  {
    $this->value = $value;
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
              "type"     => ProtocolBuffers::TYPE_STRING,
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

