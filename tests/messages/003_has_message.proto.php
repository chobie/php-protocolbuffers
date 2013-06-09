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
  protected static $descriptor;

  protected $_properties = array();

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
      if (!isset(self::$descriptor)) {
          $desc = new ProtocolBuffers_DescriptorBuilder();
          $desc->addField(1, new ProtocolBuffers_FieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_ENUM,
              "name"     => "type",
              "packable" => false,
              "repeated" => false,
              "default"  => null,
          )));
          $desc->addField(2, new ProtocolBuffers_FieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_STRING,
              "name"     => "number",
              "packable" => false,
              "repeated" => false,
              "default"  => null,
          )));

          self::$descriptor = $desc->build();
      }

      return self::$descriptor;
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
  protected static $descriptor;

  protected $_properties = array();

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
      if (!isset(self::$descriptor)) {
          $desc = new ProtocolBuffers_DescriptorBuilder();
          $desc->addField(1, new ProtocolBuffers_FieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_STRING,
              "name"     => "name",
              "packable" => false,
              "repeated" => false,
              "default"  => null,
          )));
          $desc->addField(2, new ProtocolBuffers_FieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_INT32,
              "name"     => "id",
              "packable" => false,
              "repeated" => false,
              "default"  => null,
          )));
          $desc->addField(3, new ProtocolBuffers_FieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_STRING,
              "name"     => "email",
              "packable" => false,
              "repeated" => false,
              "default"  => null,
          )));
          $desc->addField(4, new ProtocolBuffers_FieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_MESSAGE,
              "name"     => "phone",
              "packable" => false,
              "repeated" => false,
              "default"  => null,
              "message"  => "Tutorial_Person_PhoneNumber",
          )));

          self::$descriptor = $desc->build();
      }

      return self::$descriptor;
  }

}

