--TEST--
Check for protocol buffers field descriptor implementations
--FILE--
<?php

$field = new ProtocolBuffers_FieldDescriptor();
echo $field->getType() . PHP_EOL;
$field->setType(1);
echo $field->getType() . PHP_EOL;


$field = new ProtocolBuffers_FieldDescriptor(array(
    "type" => 1,
));
echo $field->getType() . PHP_EOL;

--EXPECT--
-1
1
1