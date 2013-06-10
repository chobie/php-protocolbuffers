<?php
/**
 * Tutorial_Fixed32
 *
 * @message Tutorial.Fixed32
 *
 * -*- magic properties -*-
 *
 * @property int $value
 */
class Tutorial_Fixed32
{
  protected static $descriptor;

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
              "type"     => ProtocolBuffers::TYPE_FIXED32,
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

