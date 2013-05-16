--TEST--
Check for protocol buffers simple message encoding
--FILE--
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "messages" . DIRECTORY_SEPARATOR . "003_has_message.proto.php";

$expect = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "003_has_message.bin");

$person = new Tutorial_Person();
$person->setId(20);
$person->setName("chobie");
$phone = new Tutorial_Person_PhoneNumber();
$phone->setType(1);
$phone->setNumber("0120-222-222");
$person->setPhone($phone);

if (ProtocolBuffers::encode($person) == $expect) {
    echo "OK" . PHP_EOL;
}

--EXPECT--
OK