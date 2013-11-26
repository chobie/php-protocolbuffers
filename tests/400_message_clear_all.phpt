--TEST--
Check for protocol buffers clear method implementation
--FILE--
<?php
class ClearTest extends ProtocolBuffersMessage
{
	protected $name;

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
			"name"     => "name",
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

$p = new ClearTest();
$p->setName("chobie");
$p->clearAll();

if (!$p->hasName()) {
    echo "OK" . PHP_EOL;
}

--EXPECT--
OK