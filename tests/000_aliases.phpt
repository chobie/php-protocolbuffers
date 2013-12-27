--TEST--
Check for protocol buffers sfixed64 implementations
--SKIPIF--
<?php if (version_compare(PHP_VERSION, "5.3.0") < 0) { die('skip for 5.2'); } ?>
--FILE--
<?php

$target  = new ProtocolBuffers\FieldDescriptor();
if ($target  instanceof ProtocolBuffersFieldDescriptor) {
	echo "OK" . PHP_EOL;
} else {
	echo get_class($target) . PHP_EOL;
}

$target = new ProtocolBuffers\DescriptorBuilder();
if ($target instanceof ProtocolBuffersDescriptorBuilder) {
	echo "OK" . PHP_EOL;
} else {
	echo get_class($target) . PHP_EOL;
}

$target = new ProtocolBuffers\UnknownField();
if ($target instanceof ProtocolBuffersUnknownField) {
	echo "OK" . PHP_EOL;
} else {
	echo get_class($target) . PHP_EOL;
}

$target = new ProtocolBuffers\UnknownFieldSet();
if ($target instanceof ProtocolBuffersUnknownFieldSet) {
	echo "OK" . PHP_EOL;
} else {
	echo get_class($target) . PHP_EOL;
}

$target = new ProtocolBuffers\MessageOptions();
if ($target instanceof ProtocolBuffersMessageOptions) {
	echo "OK" . PHP_EOL;
} else {
	echo get_class($target) . PHP_EOL;
}

$target = new ProtocolBuffers\PHPMessageOptions();
if ($target instanceof ProtocolBuffersPHPMessageOptions) {
	echo "OK" . PHP_EOL;
} else {
	echo get_class($target) . PHP_EOL;
}

class B implements ProtocolBuffers\Describable{public static function getDescriptor(){}}
$target = new B();
if ($target instanceof ProtocolBuffersDescribable) {
	echo "OK" . PHP_EOL;
} else {
	echo get_class($target) . PHP_EOL;
}

$target = new ProtocolBuffers\InvalidByteSequenceException();
if ($target instanceof ProtocolBuffersInvalidByteSequenceException) {
	echo "OK" . PHP_EOL;
} else {
	echo get_class($target) . PHP_EOL;
}

$target = new ProtocolBuffers\InvalidProtocolBufferException();
if ($target instanceof ProtocolBuffersInvalidProtocolBufferException) {
	echo "OK" . PHP_EOL;
} else {
	echo get_class($target) . PHP_EOL;
}

$target = new ProtocolBuffers\UninitializedMessageException();
if ($target instanceof ProtocolBuffersUninitializedMessageException) {
	echo "OK" . PHP_EOL;
} else {
	echo get_class($target) . PHP_EOL;
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
