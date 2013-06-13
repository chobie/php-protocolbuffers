--TEST--
Check for protocol buffers descriptor builder implementations
--FILE--
<?php

$builder = new ProtocolBuffersDescriptorBuilder();
$builder->setName("Person");

echo $builder->getName("Person") . PHP_EOL;
echo $builder->getName("Person") . PHP_EOL;
echo strlen($builder->getName("Person")) . PHP_EOL;

$descriptor = $builder->build();

if ($descriptor instanceof ProtocolBuffersDescriptor) {
    echo $descriptor->getName("Person") . PHP_EOL;
    echo $descriptor->getName("Person") . PHP_EOL;
    echo strlen($descriptor->getName("Person")) . PHP_EOL;

} else {
    echo "FAULT: INSTANCE IS NOT ProtocolBuffersDescriptor" . PHP_EOL;
}

--EXPECT--
Person
Person
6
Person
Person
6