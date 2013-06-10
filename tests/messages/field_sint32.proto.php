<?php
/**
 * Tutorial_SInt32
 *
 * @message Tutorial.SInt32
 *
 * -*- magic properties -*-
 *
 * @property int $value
 */
class Tutorial_SInt32
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
              "type"     => ProtocolBuffers::TYPE_SINT32,
              "name"     => "value",
              "required" => true,
              "packable" => false,
              "repeated" => false,
              "default"  => null,
          )));

          self::$descriptor = $desc->build();
      }

      return self::$descriptor;

  }

}

