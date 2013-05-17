<?php
/**
 * Tutorial_Bytes
 *
 * @message Tutorial.Bytes
 *
 * -*- magic properties -*-
 *
 * @property string $value
 */
class Tutorial_Bytes
{
  protected static $scheme = array(
    'properties' => array(
      'value' => array(
        'type' => 'string',
        'opts' => array(
          'tag' => 1,
        ),
      ),
    ),
    'scheme' => array(
      1 => array(
        'name'     => 'value',
        'type'     => 12,
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

