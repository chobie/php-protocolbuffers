<?php

/**
 * Tutorial_Person_PhoneNumber
 *
 * @message Tutorial.Person.PhoneNumber
 *
 * -*- magic properties -*-
 *
 * @property int $type
 * @property string $number
 */
class Tutorial_Person_PhoneNumber
{
  protected static $scheme = array(
    'properties' => array(
      'type' => array(
        'type' => 'int',
        'opts' => array(
          'tag' => 1,
        ),
      ),
      'number' => array(
        'type' => 'string',
        'opts' => array(
          'tag' => 2,
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

  public function getType()
  {
    $result = null;

    if (array_key_exists('type', $this->_properties)) {
      $result = $this->_properties['type'];
    }

    return $result;
  }

  public function setType($type)
  {
    $this->_properties['type'] = $type;
  }

  public function getNumber()
  {
    $result = null;

    if (array_key_exists('number', $this->_properties)) {
      $result = $this->_properties['number'];
    }

    return $result;
  }

  public function setNumber($number)
  {
    $this->_properties['number'] = $number;
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
class Tutorial_Person
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

  public function getName()
  {
    $result = null;

    if (array_key_exists('name', $this->_properties)) {
      $result = $this->_properties['name'];
    }

    return $result;
  }

  public function setName($name)
  {
    $this->_properties['name'] = $name;
  }

  public function getId()
  {
    $result = null;

    if (array_key_exists('id', $this->_properties)) {
      $result = $this->_properties['id'];
    }

    return $result;
  }

  public function setId($id)
  {
    $this->_properties['id'] = $id;
  }

  public function getEmail()
  {
    $result = null;

    if (array_key_exists('email', $this->_properties)) {
      $result = $this->_properties['email'];
    }

    return $result;
  }

  public function setEmail($email)
  {
    $this->_properties['email'] = $email;
  }

  public function getPhone()
  {
    $result = null;

    if (array_key_exists('phone', $this->_properties)) {
      $result = $this->_properties['phone'];
    }

    return $result;
  }

  public function setPhone($phone)
  {
    $this->_properties['phone'] = $phone;
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

