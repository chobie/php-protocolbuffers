--TEST--
Check for protocol buffers use single property implementations
--FILE--
<?php
class SingleProperty
{
	protected $_values;

	public function setName($name)
	{
		$this->_values["name"] = $name;
	}

	public function getName()
	{
		return $this->_values["name"];
	}

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

			$phpoptions = $desc->getOptions()->getExtension(ProtocolBuffers::PHP_MESSAGE_OPTION);
			$phpoptions->setUseSingleProperty(true);
			$phpoptions->setSinglePropertyName("_values");
			$descriptor = $desc->build();
		}

		return $descriptor;
	}
}

$p = new SingleProperty();
$p->setName("chobie");

$data = ProtocolBuffers::encode($p);
$p2 = ProtocolBuffers::decode("SingleProperty", $data);

echo $p2->getName();

--EXPECT--
chobie
