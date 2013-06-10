<?php
/**
 * Tutorial_Boolean
 *
 * @message Tutorial.Boolean
 *
 * -*- magic properties -*-
 *
 * @property array $value
 */
class Tutorial_Boolean
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
              "type"     => ProtocolBuffers::TYPE_BOOL,
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

