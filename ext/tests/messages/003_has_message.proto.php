<?php

/**
 * Tutorial_Person_PhoneNumber
 *
 * @message Tutorial.Person.PhoneNumber
 *
 * -*- magic properties -*-
 *
 * @property string $number
 * @property int $type
 */
class Tutorial_Person_PhoneNumber extends Base
{
  protected static $scheme = array(
    'properties' => array(
      'number' => array(
        'type' => 'string',
        'opts' => array(
          'tag' => 2,
        ),
      ),
      'type' => array(
        'type' => 'int',
        'opts' => array(
          'tag' => 1,
        ),
      ),
    ),
    'scheme' => array(
      1 => array(
        'name'     => 'type',
        'type'     => 14,
        'required' => false,
        'optional' => true,
        'repeated' => false,
        'packable' => false,
        'default'  => '',
      ),
      2 => array(
        'name'     => 'number',
        'type'     => 9,
        'required' => true,
        'optional' => false,
        'repeated' => false,
        'packable' => false,
        'default'  => null,
      ),
    ),
  );

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

/**
 * Tutorial_Person_PhoneType
 *
 * @enum Tutorial.Person.PhoneType
 */
class Tutorial_Person_PhoneType
{
  const MOBILE = 0;
  const HOME = 1;
  const WORK = 2;
}

/**
 * Tutorial_Person
 *
 * @message Tutorial.Person
 *
 * -*- magic properties -*-
 *
 * @property string $name
 * @property int $id
 * @property string $email
 * @property Tutorial_Person_PhoneNumber $phone
 */
class Tutorial_Person extends Base
{
  protected static $scheme = array(
    'properties' => array(
      'name' => array(
        'type' => 'string',
        'opts' => array(
          'tag' => 1,
        ),
      ),
      'id' => array(
        'type' => 'int',
        'opts' => array(
          'tag' => 2,
        ),
      ),
      'email' => array(
        'type' => 'string',
        'opts' => array(
          'tag' => 3,
        ),
      ),
      'phone' => array(
        'type' => 'Tutorial_Person_PhoneNumber',
        'opts' => array(
          'tag' => 4,
        ),
      ),
    ),
    'scheme' => array(
      1 => array(
        'name'     => 'name',
        'type'     => 9,
        'required' => true,
        'optional' => false,
        'repeated' => false,
        'packable' => false,
        'default'  => null,
      ),
      2 => array(
        'name'     => 'id',
        'type'     => 5,
        'required' => true,
        'optional' => false,
        'repeated' => false,
        'packable' => false,
        'default'  => null,
      ),
      3 => array(
        'name'     => 'email',
        'type'     => 9,
        'required' => false,
        'optional' => true,
        'repeated' => false,
        'packable' => false,
        'default'  => null,
      ),
      4 => array(
        'name'     => 'phone',
        'type'     => 11,
        'required' => true,
        'optional' => false,
        'repeated' => false,
        'packable' => false,
        'default'  => null,
        'message'  => 'Tutorial_Person_PhoneNumber',
      ),
    ),
  );

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

