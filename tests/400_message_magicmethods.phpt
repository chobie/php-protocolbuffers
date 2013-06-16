--TEST--
Check for protocol buffers unknown fields implementations
--FILE--
<?php
class SingleProperty extends ProtocolBuffersMessage
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

$p = new SingleProperty();
$p->setName("chobie");

if ($p->hasName()) {
	echo "OK" . PHP_EOL;
}
echo $p->getName() . PHP_EOL;
$p->clearName();
if (!$p->hasName()) {
	echo "OK" . PHP_EOL;
}
echo $p->getName(); // Nothing to display

--EXPECT--
OK
chobie
OK