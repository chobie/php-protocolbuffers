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
  protected static $scheme = array(
    'properties' => array(
      'id' => array(
        'type' => 'int',
        'opts' => array(
          'tag' => 1,
        ),
      ),
      'offense' => array(
        'type' => 'float',
        'opts' => array(
          'tag' => 2,
        ),
      ),
      'name' => array(
        'type' => 'string',
        'opts' => array(
          'tag' => 3,
        ),
      ),
    ),
    'scheme' => array(
      1 => array(
        'name'     => 'id',
        'type'     => 5,
        'required' => true,
        'optional' => false,
        'repeated' => false,
        'packable' => false,
        'default'  => null,
      ),
      2 => array(
        'name'     => 'offense',
        'type'     => 2,
        'required' => false,
        'optional' => true,
        'repeated' => false,
        'packable' => false,
        'default'  => null,
      ),
      3 => array(
        'name'     => 'name',
        'type'     => 9,
        'required' => false,
        'optional' => true,
        'repeated' => false,
        'packable' => false,
        'default'  => null,
      ),
    ),
  );

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
    return self::$scheme['scheme'];
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

