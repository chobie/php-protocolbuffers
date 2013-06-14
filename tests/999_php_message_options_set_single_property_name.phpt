--TEST--
Check for protocol buffers php message options set_single_property_name bug.
--FILE--
<?php
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
$phpoptions->setSinglePropertyName("_properties"); // do not segv.

$descriptor = $desc->build();
echo "OK";
--EXPECT--
OK