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
class Tutorial_Person_PhoneNumber
{
  protected static $descriptor;
  /**
   * getting value
   *
   * @return mixed $number
   */
  public function getNumber()
  {
    $result = null;

    if (array_key_exists('number', $this->_properties)) {
      $result = $this->_properties['number'];
    }

    return $result;
  }

  /**
   * setting value
   *
   * @property mixed $number
   *
   * @return void
   */
  public function setNumber($number)
  {
    $this->_properties['number'] = $number;
  }

  /**
   * getting value
   *
   * @return mixed $type
   */
  public function getType()
  {
    $result = null;

    if (array_key_exists('type', $this->_properties)) {
      $result = $this->_properties['type'];
    }

    return $result;
  }

  /**
   * setting value
   *
   * @property mixed $type
   *
   * @return void
   */
  public function setType($type)
  {
    $this->_properties['type'] = $type;
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
        "type"     => 9,
        "name"     => "number",
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => "",
      )));
      $desc->addField(2, new ProtocolBuffers_FieldDescriptor(array(
        "type"     => 14,
        "name"     => "type",
        "optional" => true,
        "repeated" => false,
        "packable" => false,
        "default"  => Tutorial_Person_PhoneType::HOME,
      )));
      self::$descriptor = $desc->build();
    }
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
 * @property array $phone
 */
class Tutorial_Person
{
  protected static $descriptor;
  /**
   * getting value
   *
   * @return mixed $name
   */
  public function getName()
  {
    $result = null;

    if (array_key_exists('name', $this->_properties)) {
      $result = $this->_properties['name'];
    }

    return $result;
  }

  /**
   * setting value
   *
   * @property mixed $name
   *
   * @return void
   */
  public function setName($name)
  {
    $this->_properties['name'] = $name;
  }

  /**
   * getting value
   *
   * @return mixed $id
   */
  public function getId()
  {
    $result = null;

    if (array_key_exists('id', $this->_properties)) {
      $result = $this->_properties['id'];
    }

    return $result;
  }

  /**
   * setting value
   *
   * @property mixed $id
   *
   * @return void
   */
  public function setId($id)
  {
    $this->_properties['id'] = $id;
  }

  /**
   * getting value
   *
   * @return mixed $email
   */
  public function getEmail()
  {
    $result = null;

    if (array_key_exists('email', $this->_properties)) {
      $result = $this->_properties['email'];
    }

    return $result;
  }

  /**
   * setting value
   *
   * @property mixed $email
   *
   * @return void
   */
  public function setEmail($email)
  {
    $this->_properties['email'] = $email;
  }

  /**
   * getting value
   *
   * @return mixed $phone
   */
  public function getPhone()
  {
    $result = null;

    if (array_key_exists('phone', $this->_properties)) {
      $result = $this->_properties['phone'];
    }

    return $result;
  }

  /**
   * setting value
   *
   * @property mixed $phone
   *
   * @return void
   */
  public function addPhone($phone)
  {
    $this->_properties['phone'][] = $phone;
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
        "type"     => 9,
        "name"     => "name",
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => "",
      )));
      $desc->addField(2, new ProtocolBuffers_FieldDescriptor(array(
        "type"     => 5,
        "name"     => "id",
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => 0,
      )));
      $desc->addField(3, new ProtocolBuffers_FieldDescriptor(array(
        "type"     => 9,
        "name"     => "email",
        "optional" => true,
        "repeated" => false,
        "packable" => false,
        "default"  => "",
      )));
      $desc->addField(4, new ProtocolBuffers_FieldDescriptor(array(
        "type"     => 11,
        "name"     => "phone",
        "optional" => false,
        "repeated" => true,
        "packable" => false,
        "default"  => null,
        "message"  => "Tutorial_Person_PhoneNumber",
      )));
      self::$descriptor = $desc->build();
    }
  }

}

/**
 * Tutorial_AddressBook
 *
 * @message Tutorial.AddressBook
 *
 * -*- magic properties -*-
 *
 * @property array $person
 */
class Tutorial_AddressBook
{
  protected static $descriptor;
  /**
   * getting value
   *
   * @return mixed $person
   */
  public function getPerson()
  {
    $result = null;

    if (array_key_exists('person', $this->_properties)) {
      $result = $this->_properties['person'];
    }

    return $result;
  }

  /**
   * setting value
   *
   * @property mixed $person
   *
   * @return void
   */
  public function addPerson($person)
  {
    $this->_properties['person'][] = $person;
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
        "type"     => 11,
        "name"     => "person",
        "optional" => false,
        "repeated" => true,
        "packable" => false,
        "default"  => null,
        "message"  => "Tutorial_Person",
      )));
      self::$descriptor = $desc->build();
    }
  }

}

