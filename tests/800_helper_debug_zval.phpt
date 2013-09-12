--TEST--
Check for ProtocolBuffers\Helper::zigzagEncode / zigzagDecode implementeation
--FILE--
<?php
// don't segv
ob_start();
ProtocolBuffersHelper::debugZval(1);
ob_end_clean();

echo "OK";
--EXPECT--
OK