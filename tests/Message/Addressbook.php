<?php
/**
 * AddressBook
 *
 * intermediate class of AddressBook.
 *
 * @author Shuhei Tanuma <chobieee@gmail.com>
 * @message AddressBook
 *
 * -*- magic properties -*-
 *
 * @property array     $user                 :tag => 1, :message => "User"
 *
 * -*- magic methods
 *
 * @method array getUser()
 */
class AddressBook
    extends Base
{
    protected static $scheme = array(
        "properties" => array(
            "user" => array(
                "type" => "User",
                "opts" => array(
                    "tag" => 1,
                ),
            ),
        ),
        "scheme" => array(
            1 => array(
                "name"      => "user",
                "type"      => ProtocolBuffers::TYPE_MESSAGE,
                "wire_type" => ProtocolBuffers::WIRETYPE_LENGTH_DELIMITED,
                "required"  => true,
                "repeated"  => true,
                "message"   => "User",
            ),
        )
    );

    public static function getProto()
    {
        return self::$scheme['scheme'];
    }

    public function __construct($array = array())
    {
        foreach ($array as $key => $value) {
            if (isset(self::$scheme['properties'][$key])) {
                $this->_properties[$key] = $value;
            } else {
                throw new InvalidArgumentException("property $key doesnt exist in proto declaration");
            }
        }
    }

    public function addUser(User $u)
    {
        $this->_properties['user'][] = $u;
    }
}
