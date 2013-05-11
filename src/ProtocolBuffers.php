<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "ProtocolBuffers/Support.php";
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "ProtocolBuffers/Packer/BinaryPacker.php";

define("kMaxVarintBytes", 10);
define("kMaxVarint32Bytes", 5);
define("TAG_TYPE_BITS", 3);
define("_WIRETYPE_MAX", 5);
define("TAG_TYPE_MASK", (1 << TAG_TYPE_BITS) - 1); // 0x7

class ProtocolBuffers
{
    const WIRETYPE_VARINT           = 0;
    const WIRETYPE_FIXED64          = 1;
    const WIRETYPE_LENGTH_DELIMITED = 2;
    const WIRETYPE_START_GROUP      = 3;
    const WIRETYPE_END_GROUP        = 4;
    const WIRETYPE_FIXED32          = 5;

    /* -*- protected constants(internal use only) -*- */

    const TYPE_DOUBLE     = 1;
    const TYPE_FLOAT      = 2;
    const TYPE_INT64      = 3;
    const TYPE_UINT64     = 4;
    const TYPE_INT32      = 5;
    const TYPE_FIXED64    = 6;
    const TYPE_FIXED32    = 7;
    const TYPE_BOOL       = 8;
    const TYPE_STRING     = 9;
    const TYPE_GROUP      = 10;
    const TYPE_MESSAGE    = 11;
    const TYPE_BYTES      = 12;
    const TYPE_UINT32     = 13;
    const TYPE_ENUM       = 14;
    const TYPE_SFIXED32   = 15;
    const TYPE_SFIXED64   = 16;
    const TYPE_SINT32     = 17;
    const TYPE_SINT64     = 18;
    const MAX_FIELED_TYPE = 18;

    protected static $packer = array();

    public static function decode($proto, $class, $buffers)
    {
        if (!self::$packer) {
            self::$packer = new ProtocolBuffers_Packer_BinaryPacker();
        }
//        $packer = self::$packer;

        $r = pb_decode($proto, $class, $buffers);
        return $r;

        $size = strlen($buffers);
        $result = array();

        $tag_buffer = "";
        for ($i = 0; $i < $size; $i++) {
            $bit = ord($buffers[$i]);
            $tag_buffer .= $bit;

            if (!($bit & 0x80)) {
                list($tag, $wiretype) = $packer->unpackTag($tag_buffer);
                $tag_buffer = "";

                if (isset($proto[$tag])) {
                    $info = $proto[$tag];
                    // forwards 1 bit.
                    $i++;

                    switch($wiretype) {
                    case ProtocolBuffers::WIRETYPE_VARINT:
                        $data = "";

                        for ($x = $i; $x < $size; $x++) {
                            if ($buffers[$x] & 0x80) {
                                $data .= $buffers[$x];
                            } else {
                                $data .= $buffers[$x];
                                break;
                            }
                        }
                        $i = $x;

                        $value = $packer->readVarint32($data);
                        $result[$info['name']] = $value;
                        break;
                    case ProtocolBuffers::WIRETYPE_FIXED32:
                        $value = substr($buffers, $i, 4);
                        if ($info['type'] == ProtocolBuffers::TYPE_FLOAT) {
                            $value = unpack('f', $value);
                            $result[$info['name']] = $value[1];
                        } else {
                            throw new Exception("{$info['type']} does not support yet");
                        }
                        $i += 3;

                        break;
                    case ProtocolBuffers::WIRETYPE_FIXED64:
                        $value = substr($buffers, $i, 8);

                        // is this correct ?
                        if ($info['type'] == ProtocolBuffers::TYPE_FLOAT) {
                            $value = unpack('f', $value);
                            $result[$info['name']] = $value[1];
                        } else {
                            throw new Exception("{$info['type']} does not support yet");
                        }
                        $i += 7;

                        break;
                    case ProtocolBuffers::WIRETYPE_LENGTH_DELIMITED:
                        $data = "";
                        for ($x = $i; $x < $size; $x++) {
                            if ($buffers[$x] & 0x80) {
                                $data .= $buffers[$x];
                            } else {
                                $data .= $buffers[$x];
                                break;
                            }
                        }

                        $i = $x+1;
                        $length = $packer->readVarint32($data);
                        $value  = substr($buffers, $i, $length);

                        if ($proto[$tag]['type'] == ProtocolBuffers::TYPE_MESSAGE) {
                            $c = $info['message'];
                            $result[$info['name']][] = self::decode($c::getProto(), $c, $value);
                        } else {
                            $result[$info['name']] = $value;
                        }
                        break;
                    }
                }
            }
        }

        return new $class($result);
    }

    /**
     * encode object to protocol buffers
     *
     * @param array $proto
     * @param Base $class
     * @return string
     * @throws RuntimeException
     */
    public static function encode($proto, $class)
    {
        $packer = new ProtocolBuffers_Packer_BinaryPacker();

        $result = "";
        foreach ($proto as $tag => $column) {

            if ($column['required'] && !array_key_exists($column['name'], $class->_properties)) {
                throw new RuntimeException("the property {$column['name']} requires but it does not exist");
            }

            if (!array_key_exists($column['name'], $class->_properties) && !$column['required']) {
                continue;
            }

            $_tag = $packer->packTag($tag, self::typeToWireType($column['type']));

            switch ($column['type']) {
            case ProtocolBuffers::TYPE_DOUBLE:
                // double, platform dependent
                $result .= $packer->writeVarint32($_tag) . pack("d*", $class->_properties[$column['name']]);
                break;
            case ProtocolBuffers::TYPE_FLOAT:
                // float, platform dependent
                $result .= $packer->writeVarint32($_tag) . pack("f*", $class->_properties[$column['name']]);
                break;
            case ProtocolBuffers::TYPE_INT32:
                $result .= $packer->writeVarint32($_tag)
                    . $packer->writeVarint32($class->_properties[$column['name']]);
                break;
            case ProtocolBuffers::TYPE_INT64:
                $result .= $packer->writeVarint32($_tag)
                    . $packer->writeVarint64($class->_properties[$column['name']]);
                break;
            case ProtocolBuffers::TYPE_UINT32:
                $result .= $packer->writeVarint32($_tag)
                    . $packer->writeVarint32($class->_properties[$column['name']]);
                break;
            case ProtocolBuffers::TYPE_UINT64:
                $result .= $packer->writeVarint32($_tag)
                    . $packer->writeVarint64($class->_properties[$column['name']]);
                break;
            case ProtocolBuffers::TYPE_FIXED32:
                // TODO: check implementation
                // unsigned long, little endianness
                $result .= $packer->writeVarint32($_tag) . pack("V*",
                    $this->_properties[$column['name']]
                );
                break;
            case ProtocolBuffers::TYPE_FIXED64:
                // TODO: check implementation
                // unsigned long, little endianness
                $result .= $packer->writeVarint32($_tag) . pack("V*",
                    $class->_properties[$column['name']]  & 0xffffffff,
                    $class->_properties[$column['name']] / (0xffffffff+1)
                );
                break;
            case ProtocolBuffers::TYPE_SFIXED32:
                // TODO: check implementation
                // signed integer, platform dependent
                $result .= $packer->writeVarint32($_tag) . pack("l*",
                    $class->_properties[$column['name']]
                );
                break;
            case ProtocolBuffers::TYPE_SFIXED64:
                // TODO: check implementation
                // unsigned long, little endianness
                $result .= $packer->writeVarint32($_tag) . pack("V*",
                    $class->_properties[$column['name']]  & 0xffffffff,
                    $class->_properties[$column['name']] / (0xffffffff+1)
                );
                break;
            case ProtocolBuffers::TYPE_BOOL:
                $result .= $packer->writeVarint32($_tag)
                    . writeVarint32((int)(bool)$class->_properties[$column['name']]);
                break;
            case ProtocolBuffers::TYPE_STRING:
                $result .= $packer->writeVarint32($_tag)
                    . $packer->writeVarint32(strlen($class->_properties[$column['name']]))
                    . $class->_properties[$column['name']];
                break;
            case ProtocolBuffers::TYPE_BYTES:
                $result .= $packer->writeVarint32($_tag)
                    . writeVarint32(strlen($class->_properties[$column['name']]))
                    . $class->_properties[$column['name']];
                break;
            case ProtocolBuffers::TYPE_MESSAGE:
                if ($column['repeated']) {
                    foreach ($class->_properties[$column['name']] as $v) {
                        $data    = self::encode($v::getProto(), $v);
                        $result .= $packer->writeVarint32($_tag)
                            .$packer->writeVarint32(strlen($data))
                            . $data;
                    }
                } else {
                    $v = $class->_properties[$column['name']];
                    $data    = self::encode($v::getProto(), $v);
                    $result .= $packer->writeVarint32($_tag)
                        . $packer->writeVarint32(strlen($data))
                        . $data;
                }
                break;
            default:
                throw new RuntimeException("unsupported type");
            }
        }

        return $result;
    }

    public static function typeToWireType($type)
    {
        $map = array(
            self::TYPE_DOUBLE   => self::WIRETYPE_FIXED32,
            self::TYPE_FLOAT    => self::WIRETYPE_FIXED32,
            self::TYPE_INT64    => self::WIRETYPE_VARINT,
            self::TYPE_UINT64   => self::WIRETYPE_VARINT,
            self::TYPE_INT32    => self::WIRETYPE_VARINT,
            self::TYPE_FIXED64  => self::WIRETYPE_FIXED64,
            self::TYPE_FIXED32  => self::WIRETYPE_FIXED32,
            self::TYPE_BOOL     => self::WIRETYPE_VARINT,
            self::TYPE_STRING   => self::WIRETYPE_LENGTH_DELIMITED,
            self::TYPE_GROUP    => null,
            self::TYPE_MESSAGE  => self::WIRETYPE_LENGTH_DELIMITED,
            self::TYPE_BYTES    => self::WIRETYPE_LENGTH_DELIMITED,
            self::TYPE_UINT32   => self::WIRETYPE_VARINT,
            self::TYPE_ENUM     => self::WIRETYPE_VARINT,
            self::TYPE_SFIXED32 => self::WIRETYPE_FIXED32,
            self::TYPE_SFIXED64 => self::WIRETYPE_FIXED64,
            self::TYPE_SINT32   => self::WIRETYPE_VARINT,
            self::TYPE_SINT64   => self::WIRETYPE_VARINT,
        );

        return $map[$type];
    }

    public static function generateProto($class)
    {
        $proto = $class::getProto();

        $map = array(
            self::TYPE_DOUBLE   => "double",
            self::TYPE_FLOAT    => "float",
            self::TYPE_INT64    => "int64",
            self::TYPE_UINT64   => "uint64",
            self::TYPE_INT32    => "int32",
            self::TYPE_FIXED64  => "fixed64",
            self::TYPE_FIXED32  => "fixed32",
            self::TYPE_BOOL     => "bool",
            self::TYPE_STRING   => "string",
            self::TYPE_GROUP    => "group",
            self::TYPE_MESSAGE  => "",
            self::TYPE_BYTES    => "bytes",
            self::TYPE_UINT32   => "uint32",
            self::TYPE_ENUM     => "",
            self::TYPE_SFIXED32 => "sfixed32",
            self::TYPE_SFIXED64 => "sfixed64",
            self::TYPE_SINT32   => "sint32",
            self::TYPE_SINT64   => "sint64",
        );

        $injection = array();
        foreach ($proto as $tag => $opts) {
            $line = array();

            if ($opts['repeated']) {
                $line[] = "repeated";
            }

            if ($opts['required']) {
                $line[] = "required";
            } else {
                $line[] = "optional";
            }

            if ($opts['type'] == self::TYPE_MESSAGE) {
                $line[] = $opts['message'];
            } else if ($opts['type'] == self::TYPE_ENUM) {
                $line[] = $opts['enum'];
            } else {
                $line[] = $map[$opts['type']];
            }

            $line[] = $opts['name'];
            $line[] = "= " . $tag;

            $injection[] = "    " . join(" ", $line) . ";";
        }
        $injection = join("\r\n", $injection);

$template = <<<EOF
message $class
{
$injection
}

EOF;
        return $template;
    }
}
