<?php
/**
 * Tutorial_User
 *
 * @message Tutorial.User
 *
 * -*- magic properties -*-
 *
 * @property int $id
 * @property float $offense
 * @property string $name
 */
class Tutorial_User
{
  protected static $descriptor;

  protected $_properties = array();

  protected $id;

  protected $offense;

  protected $name;

  public function getId()
  {
    return $this->id;
  }

  public function setId($id)
  {
    $this->id = $id;
  }

  public function getOffense()
  {
    return $this->offense;
  }

  public function setOffense($offense)
  {
    $this->offense = $offense;
  }

  public function getName()
  {
    return $this->name;
  }

  public function setName($name)
  {
    $this->name = $name;
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
              "type"     => ProtocolBuffers::TYPE_INT32,
              "name"     => "id",
              "required" => true,
              "packable" => false,
              "repeated" => false,
              "default"  => null,
          )));
          $desc->addField(2, new ProtocolBuffers_FieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_FLOAT,
              "name"     => "offense",
              "required" => false,
              "packable" => false,
              "repeated" => false,
              "default"  => null,
          )));
          $desc->addField(3, new ProtocolBuffers_FieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_STRING,
              "name"     => "name",
              "required" => false,
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
 * Tutorial_AddressBook
 *
 * @message Tutorial.AddressBook
 *
 * -*- magic properties -*-
 *
 * @property array $user
 */
class Tutorial_AddressBook
{
  protected static $descriptor;

  protected $_properties = array();

  protected $user = array();

  public function getUser()
  {
    return $this->user;
  }

  public function addUser($user)
  {
    $this->user[] = $user;
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
              "type"     => ProtocolBuffers::TYPE_MESSAGE,
              "name"     => "user",
              "required" => false,
              "packable" => false,
              "repeated" => true,
              "default"  => null,
              "message"  => "Tutorial_User",
          )));

          self::$descriptor = $desc->build();
      }

      return self::$descriptor;
  }

}

