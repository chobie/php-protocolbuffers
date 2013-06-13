--TEST--
Check for protocol buffers descriptor builder implementations (accepts empty fields)
--FILE--
<?php

$builder = new ProtocolBuffersDescriptorBuilder();
$descriptor = $builder->build();

if ($descriptor instanceof ProtocolBuffersDescriptor) {
	echo "OK";
} else {
    echo "FAULT: INSTANCE IS NOT ProtocolBuffersDescriptor" . PHP_EOL;
}

--EXPECT--
OK
