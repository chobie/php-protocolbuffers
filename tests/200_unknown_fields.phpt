--TEST--
Check for protocol buffers unknown fields implementations
--FILE--
<?php
class UnknownFields
{
    protected static $descriptor;

    protected $value;

    protected $_unknown;

    public function getUnknownFieldSet()
    {
        return $this->_unknown;
    }

    /**
     * get descriptor for protocol buffers
     *
     * @return array
     */
    public static function getDescriptor()
    {
        if (!isset(self::$descriptor)) {
            $desc = new ProtocolBuffersDescriptorBuilder();
            $phpoptions = $desc->getOptions()->getExtension(ProtocolBuffers::PHP_MESSAGE_OPTION);
            $phpoptions->setProcessUnknownFields(true);
            self::$descriptor = $desc->build();
        }

        return self::$descriptor;
    }
}

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "003_has_message.bin");

$data = ProtocolBuffers::decode("UnknownFields", $bytes);
--EXPECT--
chobie
20