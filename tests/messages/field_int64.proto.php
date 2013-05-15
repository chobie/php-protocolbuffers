<?php
/**
 * Tutorial_Integer64
 *
 * @message Tutorial.Integer64
 *
 * -*- magic properties -*-
 *
 * @property int $value
 */
class Tutorial_Integer64
{
  protected static $scheme = array(
    'properties' => array(
      'value' => array(
        'type' => 'int',
        'opts' => array(
          'tag' => 1,
        ),
      ),
    ),
    'scheme' => array(
      1 => array(
        'name'     => 'value',
        'type'     => 3,
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

