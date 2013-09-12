--TEST--
Check for protocol buffers descriptor builder setExtensionRange implementation
--FILE--
<?php

$builder = new ProtocolBuffersDescriptorBuilder();
$builder->addField(16, new ProtocolBuffersFieldDescriptor(array(
	"type"     => ProtocolBuffers::TYPE_STRING,
	"name"     => "name",
	"required" => false,
	"optional" => false,
	"repeated" => true,
	"packable" => false,
	"default"  => "",
)));

try {
    $builder->setExtensionRange(-1, 100);
} catch (InvalidArgumentException $e) {
    echo "OK" . PHP_EOL;
}

try {
    $builder->setExtensionRange(100, 1);
} catch (InvalidArgumentException $e) {
    echo "OK" . PHP_EOL;
}

try {
    $builder->setExtensionRange(10, 20);
    echo "FAULT" . PHP_EOL;
} catch (InvalidArgumentException $e) {
    echo "OK" . PHP_EOL;
}

try {
    $builder->setExtensionRange(10, 15);
    echo "OK" . PHP_EOL;
} catch (InvalidArgumentException $e) {
    echo "FAULT" . PHP_EOL;
}

--EXPECT--
OK
OK
OK
OK