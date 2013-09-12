--TEST--
Check for ProtocolBuffers\Helper::zigzagEncode / zigzagDecode implementeation
--FILE--
<?php
foreach (array(-1, -128, -256, -512, -1024) as $expected) {
    $zigzag = ProtocolBuffersHelper::zigzagEncode32($expected);
    if (ProtocolBuffersHelper::zigzagDecode32($zigzag) == $expected) {
        echo "OK" . PHP_EOL;
    }
}

--EXPECT--
OK
OK
OK
OK
OK