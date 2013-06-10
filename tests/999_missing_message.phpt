--TEST--
Check for protocol buffers decode implementations
--FILE--
<?php
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
              //"message"  => "Tutorial_User",
          )));

          self::$descriptor = $desc->build();
      }

      return self::$descriptor;
  }

}

try {
  // we expect an exception. as message parameter was not set.
  Tutorial_AddressBook::getDescriptor();
  echo "FAILED";
} catch (Exception $e) {
  echo "OK";
}

--EXPECT--
OK