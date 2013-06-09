<?php
/**
 * Tutorial_RepeatedString
 *
 * @message Tutorial.RepeatedString
 *
 * -*- magic properties -*-
 *
 * @property array $values
 */
class Tutorial_RepeatedString
{
  protected static $descriptor;

  protected $_properties = array();

  protected $values = array();

  public function getValues()
  {
    return $this->values;
  }

  public function addValues($values)
  {
    $this->values[] = $values;
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
              "type"     => ProtocolBuffers::TYPE_STRING,
              "name"     => "values",
              "packable" => false,
              "repeated" => true,
              "default"  => null,
          )));

          self::$descriptor = $desc->build();
      }

      return self::$descriptor;
  }

}

