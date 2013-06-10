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

  protected $number;
  protected $type;

  /**
   * getting value
   *
   * @return mixed $number
   */
  public function getNumber()
  {
    return $this->number;
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
    $this->number = $number;
  }

  /**
   * getting value
   *
   * @return mixed $type
   */
  public function getType()
  {
    return $this->type;
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
    $this->type = $type;
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
        "name"     => "number",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => "",
      )));
      $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_ENUM,
        "name"     => "type",
        "required" => false,
        "optional" => true,
        "repeated" => false,
        "packable" => false,
        "default"  => Tutorial_Person_PhoneType::HOME,
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
 * @property array $phone
 */
class Tutorial_Person
{
  protected static $descriptor;

  protected $name;

  protected $id;

  protected $email;

  protected $phone;

  //protected $_properties = array();

  /**
   * getting value
   *
   * @return mixed $name
   */
  public function getName()
  {
    return $this->name;
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
    $this->name = $name;
  }

  /**
   * getting value
   *
   * @return mixed $id
   */
  public function getId()
  {
    return $this->id;
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
    $this->id = $id;
  }

  /**
   * getting value
   *
   * @return mixed $email
   */
  public function getEmail()
  {
    return $this->email;
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
    $this->email = $email;
  }

  /**
   * getting value
   *
   * @return mixed $phone
   */
  public function getPhone()
  {
    return $this->phone;
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
    $this->phone[] = $phone;
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
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => "",
      )));
      $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_INT32,
        "name"     => "id",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => 0,
      )));
      $desc->addField(3, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_STRING,
        "name"     => "email",
        "required" => false,
        "optional" => true,
        "repeated" => false,
        "packable" => false,
        "default"  => "",
      )));
      $desc->addField(4, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_MESSAGE,
        "name"     => "phone",
        "required" => false,
        "optional" => false,
        "repeated" => true,
        "packable" => false,
        "default"  => null,
        "message"  => "Tutorial_Person_PhoneNumber",
      )));
      $phpoptions = $desc->getOptions()->getExtension("php");
      $phpoptions->setUseSingleProperty(false);
      $phpoptions->setSinglePropertyName("_properties");

      self::$descriptor = $desc->build();
    }

    return self::$descriptor;
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

  protected $person = array();

  /**
   * getting value
   *
   * @return mixed $person
   */
  public function getPerson()
  {
    return $this->person;
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
    $this->person[] = $person;
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
        "type"     => ProtocolBuffers::TYPE_MESSAGE,
        "name"     => "person",
        "required" => false,
        "optional" => false,
        "repeated" => true,
        "packable" => false,
        "default"  => null,
        "message"  => "Tutorial_Person",
      )));
      self::$descriptor = $desc->build();
    }

    return self::$descriptor;
  }

}

