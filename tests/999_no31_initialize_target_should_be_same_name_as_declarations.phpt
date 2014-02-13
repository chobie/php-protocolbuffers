--TEST--
#31 ProtocolBuffers::decode initialize target should be same name as declarations
--FILE--
<?php
// https://github.com/chobie/php-protocolbuffers/issues/31

class Content extends ProtocolBuffersMessage
{
    protected $id;

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
				"type"     => ProtocolBuffers::TYPE_BYTES,
				"name"     => "id",
				"required" => false,
				"optional" => true,
				"repeated" => false,
				"packable" => false,
				"default"  => null,
			)));
			$descriptor = $desc->build();
        }

        return $descriptor;
    }
}

class Container extends ProtocolBuffersMessage
{
    protected $configVersions; // WATCH THIS

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
				"type"     => ProtocolBuffers::TYPE_MESSAGE,
				"name"     => "configVersions",
				"required" => false,
				"optional" => false,
				"repeated" => true,
				"packable" => false,
				"default"  => null,
				"message"  => "Content"
			)));
			$descriptor = $desc->build();
        }

        return $descriptor;
    }
}
$content = new Content();
$content->setId("123");

$container = new Container();
$container->appendConfigVersions($content);
$buffer = $container->serializeToString();

// Don't SEGV
$c = Container::parseFromString($buffer);
--EXPECT--
