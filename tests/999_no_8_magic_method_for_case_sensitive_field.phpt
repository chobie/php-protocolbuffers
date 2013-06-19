--TEST--
#8 case sensitive field
--FILE--
<?php
// https://github.com/chobie/php-protocolbuffers/issues/8
class SensitiveField extends ProtocolBuffersMessage
{
    protected $regPlace;

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
				"type"     => ProtocolBuffers::TYPE_STRING,
				"name"     => "regPlace",
				"required" => true,
				"optional" => false,
				"repeated" => false,
				"packable" => false,
				"default"  => "",
			)));
            $descriptor = $desc->build();
        }

        return $descriptor;
    }
}


$t = new SensitiveField();
$t->setRegPlace("https://github.com/chobie/php-protocolbuffers/issues/8");
echo $t->getRegPlace();

--EXPECT--
https://github.com/chobie/php-protocolbuffers/issues/8