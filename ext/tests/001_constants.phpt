--TEST--
Check for protocol buffers constants
--FILE--
<?php
if (0 != ProtocolBuffers::WIRETYPE_VARINT) {
    echo "FAILED => ProtocolBuffers::WIRETYPE_VARINT" . PHP_EOL;
}

if (1 != ProtocolBuffers::WIRETYPE_FIXED64) {
    echo "FAILED => ProtocolBuffers::WIRETYPE_FIXED64" . PHP_EOL;
}

if (2 != ProtocolBuffers::WIRETYPE_LENGTH_DELIMITED) {
    echo "FAILED => ProtocolBuffers::WIRETYPE_LENGTH_DELIMITED" . PHP_EOL;
}

if (3 != ProtocolBuffers::WIRETYPE_START_GROUP) {
    echo "FAILED => ProtocolBuffers::WIRETYPE_START_GROUP" . PHP_EOL;
}

if (4 != ProtocolBuffers::WIRETYPE_END_GROUP) {
    echo "FAILED => ProtocolBuffers::WIRETYPE_END_GROUP" . PHP_EOL;
}

if (5 != ProtocolBuffers::WIRETYPE_FIXED32) {
    echo "FAILED => ProtocolBuffers::WIRETYPE_FIXED32" . PHP_EOL;
}

if (1 != ProtocolBuffers::TYPE_DOUBLE) {
    echo "FAILED => ProtocolBuffers::TYPE_DOUBLE" . PHP_EOL;
}

if (2 != ProtocolBuffers::TYPE_FLOAT) {
    echo "FAILED => ProtocolBuffers::TYPE_FLOAT" . PHP_EOL;
}

if (3 != ProtocolBuffers::TYPE_INT64) {
    echo "FAILED => ProtocolBuffers::TYPE_INT64" . PHP_EOL;
}

if (4 != ProtocolBuffers::TYPE_UINT64) {
    echo "FAILED => ProtocolBuffers::TYPE_UINT64" . PHP_EOL;
}

if (5 != ProtocolBuffers::TYPE_INT32) {
    echo "FAILED => ProtocolBuffers::TYPE_INT32" . PHP_EOL;
}

if (6 != ProtocolBuffers::TYPE_FIXED64) {
    echo "FAILED => ProtocolBuffers::TYPE_FIXED64" . PHP_EOL;
}

if (7 != ProtocolBuffers::TYPE_FIXED32) {
    echo "FAILED => ProtocolBuffers::TYPE_FIXED32" . PHP_EOL;
}

if (8 != ProtocolBuffers::TYPE_BOOL) {
    echo "FAILED => ProtocolBuffers::TYPE_BOOL" . PHP_EOL;
}

if (9 != ProtocolBuffers::TYPE_STRING) {
    echo "FAILED => ProtocolBuffers::TYPE_STRING" . PHP_EOL;
}

if (10 != ProtocolBuffers::TYPE_GROUP) {
    echo "FAILED => ProtocolBuffers::TYPE_GROUP" . PHP_EOL;
}

if (11 != ProtocolBuffers::TYPE_MESSAGE) {
    echo "FAILED => ProtocolBuffers::TYPE_MESSAGE" . PHP_EOL;
}

if (12 != ProtocolBuffers::TYPE_BYTES) {
    echo "FAILED => ProtocolBuffers::TYPE_BYTES" . PHP_EOL;
}

if (13 != ProtocolBuffers::TYPE_UINT32) {
    echo "FAILED => ProtocolBuffers::TYPE_UINT32" . PHP_EOL;
}

if (14 != ProtocolBuffers::TYPE_ENUM) {
    echo "FAILED => ProtocolBuffers::TYPE_ENUM" . PHP_EOL;
}

if (15 != ProtocolBuffers::TYPE_SFIXED32) {
    echo "FAILED => ProtocolBuffers::TYPE_SFIXED32" . PHP_EOL;
}

if (16 != ProtocolBuffers::TYPE_SFIXED64) {
    echo "FAILED => ProtocolBuffers::TYPE_SFIXED64" . PHP_EOL;
}

if (17 != ProtocolBuffers::TYPE_SINT32) {
    echo "FAILED => ProtocolBuffers::TYPE_SINT32" . PHP_EOL;
}

if (18 != ProtocolBuffers::TYPE_SINT64) {
    echo "FAILED => ProtocolBuffers::TYPE_SINT64" . PHP_EOL;
}
--EXPECT--