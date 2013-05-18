<?php
/**
 * Tutorial_SFixed32
 *
 * @message Tutorial.SFixed32
 *
 * -*- magic properties -*-
 *
 * @property array $value
 */
class Tutorial_SFixed32
{
  protected static $scheme = array(
    'properties' => array(
      'value' => array(
        'type' => 'array',
        'opts' => array(
          'tag' => 1,
        ),
      ),
    ),
    'scheme' => array(
      1 => array(
        'name'     => 'value',
        'type'     => 15,
        'required' => false,
        'optional' => false,
        'repeated' => true,
        'packable' => true,
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
    return self::$scheme['scheme'];
  }

}

