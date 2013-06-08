--TEST--
Check for protocol buffers simple message encoding
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "003_has_message.proto.php";

$expect = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "003_has_message.bin");


$person = ProtocolBuffers::decode("Tutorial_Person", $expect);

echo $person->getId() . PHP_EOL;
echo $person->getName() . PHP_EOL;
echo $person->getPhone()->getType() . PHP_EOL;
echo $person->getPhone()->getNumber() . PHP_EOL;
--EXPECT--
20
chobie
1
0120-222-222