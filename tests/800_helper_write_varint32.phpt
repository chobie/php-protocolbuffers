--TEST--
Check for ProtocolBuffers\Helper::writeVarint32 implementeation
--FILE--
<?php
$varint = ProtocolBuffersHelper::writeVarint32(1);
if (_readvarint32($varint) == 1) {
    echo "OK" . PHP_EOL;
}

$varint = ProtocolBuffersHelper::writeVarint32(128);
if (_readvarint32($varint) == 128) {
    echo "OK" . PHP_EOL;
}

$varint = ProtocolBuffersHelper::writeVarint32(256);
if (_readvarint32($varint) == 256) {
    echo "OK" . PHP_EOL;
}

$varint = ProtocolBuffersHelper::writeVarint32(512);
if (_readvarint32($varint) == 512) {
    echo "OK" . PHP_EOL;
}

function _readvarint32($value)
{
    $b = ord($value[0]);$result  = ($b & 0x7F)      ;if (!($b & 0x80)) return $result;
    $b = ord($value[1]);$result |= ($b & 0x7F) << 7 ;if (!($b & 0x80)) return $result;
    $b = ord($value[2]);$result |= ($b & 0x7F) << 14;if (!($b & 0x80)) return $result;
    $b = ord($value[3]);$result |= ($b & 0x7F) << 21;if (!($b & 0x80)) return $result;
    $b = ord($value[4]);$result |= $b          << 28;if (!($b & 0x80)) return $result;

    return $result;
}

--EXPECT--
OK
OK
OK
OK