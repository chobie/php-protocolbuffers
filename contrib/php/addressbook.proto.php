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
class Tutorial_Person_PhoneNumber extends ProtocolBuffersMessage
{

  protected $_properties = array();

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
      $phpoptions = $desc->getOptions()->getExtension(ProtocolBuffers::PHP_MESSAGE_OPTION);
      $phpoptions->setUseSingleProperty(true);
      $phpoptions->setSinglePropertyName("_properties");

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
      $desc->setExtensionRange(100, 536870912);
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
 * -*- magic properties -*-
 *
 * @property array $person
 */
class Tutorial_AddressBook extends ProtocolBuffersMessage
{

  protected $_properties = array();

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
      $phpoptions = $desc->getOptions()->getExtension(ProtocolBuffers::PHP_MESSAGE_OPTION);
      $phpoptions->setUseSingleProperty(true);
      $phpoptions->setSinglePropertyName("_properties");

      $descriptor = $desc->build();
    }

    return $descriptor;
  }

}

$__extension_registry = ProtocolBuffersExtensionRegistry::getInstance();
$__extension_registry->add('Tutorial_Person', 100, new ProtocolBuffersFieldDescriptor(array(
  "type"     => ProtocolBuffers::TYPE_STRING,
  "name"     => "fullname",
  "required" => false,
  "optional" => true,
  "repeated" => false,
  "packable" => false,
  "default"  => "",
)));
unset($__extension_registry);
