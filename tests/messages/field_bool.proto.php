<?php
/**
 * Tutorial_Boolean
 *
 * @message Tutorial.Boolean
 *
 * -*- magic properties -*-
 *
 * @property bool $value
 */
class Tutorial_Boolean
{
  protected static $scheme = array(
    'properties' => array(
      'value' => array(
        'type' => 'bool',
        'opts' => array(
          'tag' => 1,
        ),
      ),
    ),
    'scheme' => array(
      1 => array(
        'name'     => 'value',
        'type'     => 8,
        'required' => true,
        'optional' => false,
        'repeated' => false,
        'packable' => false,
        'default'  => null,
      ),
    ),
  );

  public function getValue()
  {
    $result = null;

    if (array_key_exists('value', $this->_properties)) {
      $result = $this->_properties['value'];
    }

    return $result;
  }

  public function setValue($value)
  {
    $this->_properties['value'] = $value;
  }

  /**
   * get descriptor for protocol buffers
   * 
   * @return array
   */
  public static function getDescriptor()
  {
    return self::$scheme['scheme'];
  }

}

