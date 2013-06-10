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

  protected $type;

  protected $number;

  public function getType()
  {
    return $this->type;
  }

  public function setType($type)
  {
    $this->type = $type;
  }

  public function getNumber()
  {
    return $this->number;
  }

  public function setNumber($number)
  {
    $this->number = $number;
  }

  /**
   * get descriptor for protocol buffers
   * 
   * @return array
   */
  public static function getDescriptor()
  {
      if (!isset(self::$descriptor)) {
          $desc = new ProtocolBuffersDescriptorBuilder();
          $desc->addField(1, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_ENUM,
              "name"     => "type",
              "packable" => false,
              "repeated" => false,
              "default"  => null,
          )));
          $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
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

  protected $name;

  protected $id;

  protected $phone;

  protected $email;

  public function getName()
  {
    return $this->name;
  }

  public function setName($name)
  {
    $this->name = $name;
  }

  public function getId()
  {
    return $this->id;
  }

  public function setId($id)
  {
    $this->id = $id;
  }

  public function getEmail()
  {
    return $this->email;
  }

  public function setEmail($email)
  {
    $this->email = $email;
  }

  public function getPhone()
  {
    return $this->phone;
  }

  public function setPhone($phone)
  {
    $this->phone = $phone;
  }

  /**
   * get descriptor for protocol buffers
   * 
   * @return array
   */
  public static function getDescriptor()
  {
      if (!isset(self::$descriptor)) {
          $desc = new ProtocolBuffersDescriptorBuilder();
          $desc->addField(1, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_STRING,
              "name"     => "name",
              "packable" => false,
              "repeated" => false,
              "default"  => null,
          )));
          $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_INT32,
              "name"     => "id",
              "packable" => false,
              "repeated" => false,
              "default"  => null,
          )));
          $desc->addField(3, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_STRING,
              "name"     => "email",
              "packable" => false,
              "repeated" => false,
              "default"  => null,
          )));
          $desc->addField(4, new ProtocolBuffersFieldDescriptor(array(
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

