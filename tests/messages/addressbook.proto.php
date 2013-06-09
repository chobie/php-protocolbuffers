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

  public function getOffense()
  {
    $result = null;

    if (array_key_exists('offense', $this->_properties)) {
      $result = $this->_properties['offense'];
    }

    return $result;
  }

  public function setOffense($offense)
  {
    $this->_properties['offense'] = $offense;
  }

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

  public function getUser()
  {
    $result = null;

    if (array_key_exists('user', $this->_properties)) {
      $result = $this->_properties['user'];
    }

    return $result;
  }

  public function addUser($user)
  {
    $this->_properties['user'][] = $user;
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

