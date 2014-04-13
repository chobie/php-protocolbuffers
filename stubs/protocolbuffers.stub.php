<?php
namespace {

    class ProtocolBuffers
    {
        const WIRETYPE_VARINT = 0;
        const WIRETYPE_FIXED64 = 1;
        const WIRETYPE_LENGTH_DELIMITED = 2;
        const WIRETYPE_START_GROUP = 3;
        const WIRETYPE_END_GROUP = 4;
        const WIRETYPE_FIXED32 = 5;

        const TYPE_DOUBLE = 1;
        const TYPE_FLOAT = 2;
        const TYPE_INT64 = 3;
        const TYPE_UINT64 = 4;
        const TYPE_INT32 = 5;
        const TYPE_FIXED64 = 6;
        const TYPE_FIXED32 = 7;
        const TYPE_BOOL = 8;
        const TYPE_STRING = 9;
        const TYPE_GROUP = 10;
        const TYPE_MESSAGE = 11;
        const TYPE_BYTES = 12;
        const TYPE_UINT32 = 13;
        const TYPE_ENUM = 14;
        const TYPE_SFIXED32 = 15;
        const TYPE_SFIXED64 = 16;
        const TYPE_SINT32 = 17;
        const TYPE_SINT64 = 18;
        const MAX_FIELD_TYPE = 18;
        const PHP_MESSAGE_OPTION = 0;

        /**
         * decode bytes to specified message.
         *
         * NOTE: you should use ProtocolBuffersMessage::parseFromString() instead of this method.
         *       this purpose are 5.2 or unserialize message dynamicaly
         *
         * @param $message_name message class name
         * @param $bytes
         * @param null $descriptor (deprecated)
         * @return ProtocolBuffersMessage
         */
        public static function decode($message_name, $bytes, $descriptor = NULL){}

        /**
         * serialize message to string.
         *
         * NOTE: you should use ProtocolBuffersMessage::parseFromString() instead of this method.
         *       this purpose are 5.2 or serialize message dynamicaly
         *
         * @param $message message object
         * @param null $descriptor (deprecated)
         * @return string
         */
        public static function encode($message, $descriptor = NULL){}
    }

    abstract class ProtocolBuffersMessage implements Iterator, Traversable, ProtocolBuffersDescribable, \JsonSerializable
    {
        /**
         * create new message
         *
         * @param array $data assign acceptable values to this object.
         */
        public function __construct($data = NULL){}

        /**
         * serialize this message into string
         *
         * @return string
         * @api
         */
        public function serializeToString(){}

        /**
         * parse bytes and returns new instance.
         *
         * @param $bytes
         *
         * @return self
         * @api
         */
        public static function parseFromString($bytes){}

        /**
         * merge specified message into this message.
         *
         * @param $message same message class
         */
        public function mergeFrom($message){}

        /**
         * clear specified field
         *
         * @param string $field_name
         * @api
         */
        public function clear($field_name){}

        /**
         * clear all fields
         * @api
         */
        public function clearAll(){}

        /**
         * check specified key whether exists or not.
         *
         * @param string $field_name
         * @return bool
         * @api
         */
        public function has($field_name){}

        /**
         * get specified field
         *
         * NOTE: getter method will return new child inmutable message when message doesn't have a child message.
         *       this behavior is protocol buffers design.
         *       you have to check with `has` method when you want to know specified child message whether exists or not.
         *
         * @param string $field_name
         * @return mixed
         * @api
         */
        public function get($field_name){}

        /**
         * create a child message and return it.
         *
         * @param $field_name
         * @return ProtocolBuffersMessage
         */
        public function mutable($field_name){}

        /**
         * @param $field_name
         * @param $value
         * @api
         */
        public function set($field_name, $value){}

        /**
         * batch assign method
         *
         * @param array $params
         */
        public function setFrom(array $params){}

        /**
         * append value to repeated field
         *
         * @param mixed $value
         */
        public function append($value){}

        public function hasExtension($extension){}

        /**
         * @param $extension
         * @return mixed $extension
         */
        public function getExtension($extension){}

        /**
         * clear unknown fields
         */
        public function discardUnknownFields(){}

        /**
         * @return ProtocolBuffersUnknownFieldSet
         */
        public function getUnknownFieldSet(){}

        /**
         * return parent message
         *
         * @return ProtocolBuffersMessage
         */
        public function containerOf(){}

        public function current(){}
        public function key(){}
        public function next(){}
        public function rewind(){}
        public function valid(){}

        /**
         * magic method for get, set, clear, mutable and append.
         *
         * naming conventions:
         *   method: Camel Case
         *   field: snake case
         *
         * e.g)
         *   getPhoneNumber as get("phone_number")
         *   clearPhoneNumber as clear("phone_number")
         *   mutablePhone as mutable("phone")
         */
        public function __call($name, $params){}

        /**
         * @return ProtocolBuffersDescriptor
         */
        public static function getDescriptor(){}

        /**
         * @return array
         */
        public function jsonSerialize(){}

        /**
         * @return array
         */
        public function toArray(){}
    }

    abstract class ProtocolBuffersEnum implements ProtocolBuffersEnumDescribable
    {
        public function getName($value){}
        public function isValid($value){}

        /**
         * @return ProtocolBuffersEnumDescriptor
         */
        abstract static function getEnumDescriptor();
    }

    class ProtocolBuffersUnknownFieldSet
    {
        public function count(){}

        /**
         * @param $field_number
         * @return ProtocolBuffersUnknownField
         */
        public function getField($field_number){}

        public function addField($field_number, $value){}

        /**
         * @return ProtocolBuffersUnknownField
         */
        public function current(){}
        public function key(){}
        public function next(){}
        public function rewind(){}
        public function valid(){}
    }

    class ProtocolBuffersUnknownField
    {
        /**
         * @return int
         */
        public function getNumber(){}

        /**
         * @return int
         */
        public function getType(){}

        public function isVarint(){}
        public function isFixed64(){}
        public function isLengthDelimited(){}
        public function isFixed32(){}

        /**
         * @return array
         */
        public function getAsVarintList(){}

        /**
         * @return array
         */
        public function getAsLengthDelimitedList(){}

        /**
         * @return array
         */
        public function getAsFloatList(){}

        /**
         * @return array
         */
        public function getAsDoubleList(){}

        /**
         * @return array
         */
        public function getAsFixed32List(){}

        /**
         * @return array
         */
        public function getAsFixed64List(){}
    }

    class ProtocolBuffersInvalidByteSequenceException extends Exception{}
    class ProtocolBuffersInvalidProtocolBufferException extends Exception{}
    class ProtocolBuffersUninitializedMessageException extends Exception{}

    class ProtocolBuffersDescriptor{
    }

    class ProtocolBuffersFieldDescriptor{
    }

    class ProtocolBuffersDescriptorBuilder{

        public function addField($tag, ProtocolBuffersFieldDescriptor $field_descriptor){}

        /**
         * @return ProtocolBuffersDescriptor
         */
        public function build(){}
    }

    class ProtocolBuffersEnumDescriptor {
    }

    class ProtocolBuffersEnumDescriptorBuilder{

        public function addValue(ProtocolBuffersEnumValueDescriptor $value, $force_add = false) {}

        /**
         * @return ProtocolBuffersEnumDescriptor
         */
        public function build() {}
    }

    class ProtocolBuffersEnumValueDescriptor{
    }

    interface ProtocolBuffersEnumDescribable{
        public static function getEnumDescriptor();
    }

    interface ProtocolBuffersDescribable{
        public static function getDescriptor();
    }
}


namespace ProtocolBuffers
{
    /** NOTE: All ProtocolBuffers classes has namespace'd aliases */

    class ProtocolBuffers extends \ProtocolBuffers{}
    class Message extends \ProtocolBuffersMessage{}
    class InvalidByteSequenceException extends \ProtocolBuffersInvalidByteSequenceException{}
    class InvalidProtocolBufferException extends \ProtocolBuffersInvalidProtocolBufferException{}
    class UninitializedMessageException extends \ProtocolBuffersUninitializedMessageException{}
    class Descriptor extends \ProtocolBuffersDescriptor{}
    class FieldDescriptor extends \ProtocolBuffersFieldDescriptor{}
    class DescriptorBuilder extends \ProtocolBuffersDescriptorBuilder{}
    class EnumDescriptor extends \ProtocolBuffersEnumDescriptor{}
    class EnumDescriptorBuilder extends \ProtocolBuffersEnumDescriptorBuilder{}
    class EnumValueDescriptor extends \ProtocolBuffersEnumValueDescriptor{}
    abstract class Enum extends \ProtocolBuffersEnum{}
}