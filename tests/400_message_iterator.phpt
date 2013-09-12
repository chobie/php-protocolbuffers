--TEST--
Check for ProtocolBuffers\Message iterator implementation
--FILE--
<?php
class Properties extends ProtocolBuffersMessage
{
	protected $name;
	protected $age;
	protected $lucky_numbers;

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
			$desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
			"type"     => ProtocolBuffers::TYPE_UINT32,
			"name"     => "age",
			"required" => true,
			"optional" => false,
			"repeated" => false,
			"packable" => false,
			"default"  => "",
			)));
			$desc->addField(3, new ProtocolBuffersFieldDescriptor(array(
			"type"     => ProtocolBuffers::TYPE_UINT32,
			"name"     => "lucky_numbers",
			"required" => false,
			"optional" => false,
			"repeated" => true,
			"packable" => false,
			"default"  => "",
			)));
			$descriptor = $desc->build();
		}

		return $descriptor;
	}
}

$properties = new Properties(array(
	"name" => "chobie",
	"age" => 29,
	"lucky_numbers" => array(
		1, 2, 3, 4, 5
	),
));

foreach ($properties as $prop) {
	if (is_array($prop)) {
		foreach ($prop as $p) {
			echo $p . PHP_EOL;
		}
	} else {
		echo $prop . PHP_EOL;
	}
}

--EXPECT--
chobie
29
1
2
3
4
5