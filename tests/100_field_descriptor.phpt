--TEST--
Check for protocol buffers field descriptor implementations
--FILE--
<?php

$field = new ProtocolBuffersFieldDescriptor(array("type" => 1));
echo $field->getType() . PHP_EOL;


$field = new ProtocolBuffersFieldDescriptor(array(
    "type" => 1,
));
echo $field->getType() . PHP_EOL;

--EXPECT--
1
1
