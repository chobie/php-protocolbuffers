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
  protected static $scheme = array(
    'properties' => array(
      'values' => array(
        'type' => 'array',
        'opts' => array(
          'tag' => 1,
        ),
      ),
    ),
    'scheme' => array(
      1 => array(
        'name'     => 'values',
        'type'     => 9,
        'required' => false,
        'optional' => false,
        'repeated' => true,
        'packable' => false,
        'default'  => null,
      ),
    ),
  );

  public function getValues()
  {
    $result = null;

    if (array_key_exists('values', $this->_properties)) {
      $result = $this->_properties['values'];
    }

    return $result;
  }

  public function addValues($values)
  {
    $this->_properties['values'][] = $values;
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

