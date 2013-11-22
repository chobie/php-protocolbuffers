<?php
/**
 * chobie_github_io_Field
 *
 * @message chobie.github.io.Field
 *
 */
class chobie_github_io_Field extends ProtocolBuffersMessage
{
    protected $v_double;

    protected $v_float;

    protected $v_int64;

    protected $v_uint64;

    protected $v_int32;

    protected $v_fixed64;

    protected $v_fixed32;

    protected $v_bool;

    protected $v_string;

    protected $v_bytes;

    protected $v_uint32;

    protected $v_sfixed32;

    protected $v_sfixed64;

    protected $v_sint32;

    protected $v_sint64;

    protected $v_int64_min;

    protected $v_int32_min;

    protected $v_sfixed32_min;

    protected $v_sfixed64_min;

    protected $v_sint32_min;

    protected $v_sint64_min;


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
                "type"     => ProtocolBuffers::TYPE_DOUBLE,
                "name"     => "v_double",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_FLOAT,
                "name"     => "v_float",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(3, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_INT64,
                "name"     => "v_int64",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(4, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_UINT64,
                "name"     => "v_uint64",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(5, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_INT32,
                "name"     => "v_int32",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(6, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_FIXED64,
                "name"     => "v_fixed64",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(7, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_FIXED32,
                "name"     => "v_fixed32",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(8, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_BOOL,
                "name"     => "v_bool",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => false,
            )));
            $desc->addField(9, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_STRING,
                "name"     => "v_string",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => "",
            )));
            $desc->addField(12, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_BYTES,
                "name"     => "v_bytes",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => "",
            )));
            $desc->addField(13, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_UINT32,
                "name"     => "v_uint32",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(15, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_SFIXED32,
                "name"     => "v_sfixed32",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(16, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_SFIXED64,
                "name"     => "v_sfixed64",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(17, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_SINT32,
                "name"     => "v_sint32",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(18, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_SINT64,
                "name"     => "v_sint64",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(20, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_INT64,
                "name"     => "v_int64_min",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(21, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_INT32,
                "name"     => "v_int32_min",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(22, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_SFIXED32,
                "name"     => "v_sfixed32_min",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(23, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_SFIXED64,
                "name"     => "v_sfixed64_min",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(24, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_SINT32,
                "name"     => "v_sint32_min",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $desc->addField(25, new ProtocolBuffersFieldDescriptor(array(
                "type"     => ProtocolBuffers::TYPE_SINT64,
                "name"     => "v_sint64_min",
                "required" => false,
                "optional" => true,
                "repeated" => false,
                "packable" => false,
                "default"  => 0,
            )));
            $descriptor = $desc->build();
        }

        return $descriptor;
    }

}

