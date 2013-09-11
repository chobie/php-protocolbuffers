--TEST--
Check for ProtocolBuffers\Message::appendNNN method
--FILE--
<?php

/**
 * Tutorial_Person_PhoneNumber
 *
 * @message Tutorial.Person.PhoneNumber
 *
 */
class Tutorial_Person_PhoneNumber extends ProtocolBuffersMessage
{
  protected $number;

  protected $type;


  /**
   * get descriptor for protocol buffers
   *
   * @return array
   */
  public static function getDescriptor()
  {
    static $descriptor;

    if (!isset($descriptor)) {
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
      $descriptor = $desc->build();
    }

    return $descriptor;
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
  protected $name;

  protected $id;

  protected $email;

  protected $phone = array();

    public function addPhone($a)
    {
        $this->phone[] = $a;
    }

  /**
   * get descriptor for protocol buffers
   *
   * @return array
   */
  public static function getDescriptor()
  {
    static $descriptor;

    if (!isset($descriptor)) {
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
      $descriptor = $desc->build();
    }

    return $descriptor;
  }

}

/**
 * Tutorial_AddressBook
 *
 * @message Tutorial.AddressBook
 *
 */
class Tutorial_AddressBook extends ProtocolBuffersMessage
{
  protected $person = array();


  /**
   * get descriptor for protocol buffers
   *
   * @return array
   */
  public static function getDescriptor()
  {
    static $descriptor;

    if (!isset($descriptor)) {
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
      $descriptor = $desc->build();
    }

    return $descriptor;
  }

}


$person = new Tutorial_Person();
$person->setId(1);
$person->setName("John Doe");
$person->setEmail("john.doe@example.com");

$phone = new Tutorial_Person_PhoneNumber();
$phone->setNumber("1234 5 67 78");
$phone->setType(Tutorial_Person_PhoneType::HOME);
$person->appendPhone($phone);

// $phone should be keep. (correct refcounting)
echo $phone->getNumber();
--EXPECT--
1234 5 67 78