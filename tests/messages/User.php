<?php
/**
 * User
 *
 * intermediate class of User.
 *
 * @author Shuhei Tanuma <chobieee@gmail.com>
 * @message User
 *
 * -*- magic properties -*-
 *
 * @property int     $id                 :required, :tag => 1
 * @property float   $offense            :tag => 2
 * @property string  $name               :tag => 3
 *
 * -*- magic methods
 *
 * @method int getId()
 * @method void setId($id)
 * @method int getOffense()
 * @method void setOffense($offense)
 * @method string getName()
 * @method void setName($name)
 */
class User extends Base
{
    protected static $scheme = array(
        "properties" => array(
            "id" => array(
                "type" => "integer",
                "opts" => array(
                    "tag" => 1,
                ),
            ),
            "offense" => array(
                "type" => "integer",
                "opts" => array(
                    "tag" => 2,
                ),
            ),
            "name" => array(
                "type" => "string",
                "opts" => array(
                    "tag" => 3,
                ),
            )
        ),
        "scheme" => array(
            1 => array(
                "name"      => "id",
                "type"      => ProtocolBuffers::TYPE_INT32,
                "wire_type" => ProtocolBuffers::WIRETYPE_VARINT,
                "required"  => true,
                "repeated"  => false,
            ),
            2 => array(
                "name"      => "offense",
                "type"      => ProtocolBuffers::TYPE_FLOAT,
                "wire_type" => ProtocolBuffers::WIRETYPE_FIXED32,
                "required"  => false,
                "repeated"  => false,
            ),
            3 => array(
                "name"      => "name",
                "type"      => ProtocolBuffers::TYPE_STRING,
                "wire_type" => ProtocolBuffers::WIRETYPE_LENGTH_DELIMITED,
                "required"  => false,
                "repeated"  => false,
            ),
        )
    );

    public static function getDescriptor()
    {
        return self::$scheme['scheme'];
    }
}
