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

  protected $_properties = array();

  /**
   * checking value
   *
   * @return bool
   */
  public function hasNumber()
  {
    if (isset($this->_properties['number'])) {
      return true;
    }

    return false;
  }

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
   * checking value
   *
   * @return bool
   */
  public function hasType()
  {
    if (isset($this->_properties['type'])) {
      return true;
    }

    return false;
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
      $phpoptions = $desc->getOptions()->getExtension("php");
      $phpoptions->setUseSingleProperty(true);
      $phpoptions->setSinglePropertyName("_properties");

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
 */
class Tutorial_Person extends ProtocolBuffersMessage
{
  protected static $descriptor;

  protected $name;

  protected $id;

  protected $email;

  protected $phone = array();

  protected $_unknown = array();


  public function unk($key, $bytes)
  {
      $this->_unknown[$key] = $bytes;
  }


  /**
   * checking value
   *
   * @return bool
   */
  public function hasName()
  {
    if (isset($this->name)) {
      return true;
    }

    return false;
  }

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
   * checking value
   *
   * @return bool
   */
  public function hasId()
  {
    if (isset($this->id)) {
      return true;
    }

    return false;
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
   * checking value
   *
   * @return bool
   */
  public function hasEmail()
  {
    if (isset($this->email)) {
      return true;
    }

    return false;
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
   * checking value
   *
   * @return bool
   */
  public function hasPhone()
  {
    if (isset($this->phone)) {
      return true;
    }

    return false;
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
      $phpoptions->setProcessUnknownFields(true);

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

  protected $facebook = array();

  /**
   * checking value
   *
   * @return bool
   */
  public function hasPerson()
  {
    if (isset($this->facebook['person'])) {
      return true;
    }

    return false;
  }

  /**
   * getting value
   *
   * @return mixed $person
   */
  public function getPerson()
  {
    $result = null;

    if (array_key_exists('person', $this->facebook)) {
      $result = $this->facebook['person'];
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
    $this->facebook['person'][] = $person;
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
      $phpoptions = $desc->getOptions()->getExtension("php");
      $phpoptions->setUseSingleProperty(true);
      $phpoptions->setSinglePropertyName("facebook");

      self::$descriptor = $desc->build();
    }

    return self::$descriptor;
  }

}

