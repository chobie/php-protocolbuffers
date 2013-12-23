--TEST--
#25 setter getter methods consider camel case fields.
--FILE--
<?php
// https://github.com/chobie/php-protocolbuffers/issues/25
class SensitiveField extends ProtocolBuffersMessage
{
    protected $IsDraft;

    protected $from_box_id;

    protected $TagId;

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
				"type"     => ProtocolBuffers::TYPE_BOOL,
				"name"     => "IsDraft",
				"required" => false,
				"optional" => true,
				"repeated" => false,
				"packable" => false,
				"default"  => false,
			)));
			$desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
				"type"     => ProtocolBuffers::TYPE_INT32,
				"name"     => "from_box_id",
				"required" => false,
				"optional" => true,
				"repeated" => false,
				"packable" => false,
				"default"  => false,
			)));
			$desc->addField(3, new ProtocolBuffersFieldDescriptor(array(
				"type"     => ProtocolBuffers::TYPE_INT32,
				"name"     => "TagId",
				"required" => false,
				"optional" => true,
				"repeated" => true,
				"packable" => false,
				"default"  => false,
			)));
			$descriptor = $desc->build();
        }

        return $descriptor;
    }
}

$t = new SensitiveField(array("IsDraft" => true));
if ($t->getIsDraft()) {
	echo "OK" . PHP_EOL;
} else {
	echo "FAULT" . PHP_EOL;
}
if ($t->hasIsDraft()) {
	echo "OK" . PHP_EOL;
} else {
	echo "FAULT" . PHP_EOL;
}
$t->setIsDraft(false);
if (!$t->getIsDraft()) {
	echo "OK" . PHP_EOL;
} else {
	echo "FAULT" . PHP_EOL;
}
$t->setFrom(array("IsDraft" => true, "from_box_id" => 1, "TagId" => array(1, 2)));
if ($t->getIsDraft()) {
	echo "OK" . PHP_EOL;
} else {
	echo "FAULT" . PHP_EOL;
}
if ($t->getFromBoxId() == 1) {
	echo "OK" . PHP_EOL;
} else {
	echo "FAULT" . PHP_EOL;
}
if ($t->getTagId() == array(1, 2)) {
	echo "OK" . PHP_EOL;
} else {
	echo "FAULT" . PHP_EOL;
}



$t = new SensitiveField();
$t->set("IsDraft", true);
if ($t->getIsDraft()) {
	echo "OK" . PHP_EOL;
} else {
	echo "FAULT" . PHP_EOL;
}
if ($t->get("IsDraft")) {
	echo "OK" . PHP_EOL;
} else {
	echo "FAULT" . PHP_EOL;
}
if ($t->has("IsDraft")) {
	echo "OK" . PHP_EOL;
} else {
	echo "FAULT" . PHP_EOL;
}
$t->set("from_box_id", 1);
if ($t->get("from_box_id") == 1) {
	echo "OK" . PHP_EOL;
} else {
	echo "FAULT" . PHP_EOL;
}
$t->append("TagId", 1);
$t->append("TagId", 2);
if ($t->get("TagId") == array(1, 2)) {
	echo "OK" . PHP_EOL;
} else {
	echo "FAULT" . PHP_EOL;
}


--EXPECT--
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK