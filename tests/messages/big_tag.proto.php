<?php
/**
 * Tutorial_BigTag
 *
 * @message Tutorial.BigTag
 *
 * -*- magic properties -*-
 *
 * @property int $value
 */
class Tutorial_BigTag
{
  protected static $scheme = array(
    'properties' => array(
      'value' => array(
        'type' => 'int',
        'opts' => array(
          'tag' => 536870911,
        ),
      ),
    ),
    'scheme' => array(
      536870911 => array(
        'name'     => 'value',
        'type'     => 5,
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

