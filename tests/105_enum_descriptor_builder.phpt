--TEST--
Check for protocol buffers enum descriptor builder implementations
--FILE--
<?php

$builder = new ProtocolBuffersEnumDescriptorBuilder();
$builder->addValue(new ProtocolBuffersEnumValueDescriptor(array(
	"name" => "PREFECTURE",
	"value" => 1,
)));


$descriptor = $builder->build();

if ($descriptor instanceof ProtocolBuffersEnumDescriptor) {
	echo "OK" . PHP_EOL;
} else {
    echo "FAULT: INSTANCE IS NOT ProtocolBuffersEnumDescriptor" . PHP_EOL;
}

--EXPECT--
OK