<?php
/**
 * Tutorial_Double
 *
 * @message Tutorial.Double
 *
 * -*- magic properties -*-
 *
 * @property array $value
 */
class Tutorial_Double
{
  protected static $descriptor;

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
          $desc = new ProtocolBuffers_DescriptorBuilder();
          $desc->addField(1, new ProtocolBuffers_FieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_DOUBLE,
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

