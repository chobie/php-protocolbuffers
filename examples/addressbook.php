<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . join(DIRECTORY_SEPARATOR, array("php", "addressbook.proto.php"));

$db = "addressbook.db";

$addressbook = load($db);
$person = new Tutorial_Person();
$person->setId(1);
$person->setName("John Doe");
$person->setEmail("john.doe@example.com");

$phone = new Tutorial_Person_PhoneNumber();
$phone->setNumber("1234 5 67 78");
$phone->setType(Tutorial_Person_PhoneType::HOME);
$person->appendPhone($phone);


$phone = new Tutorial_Person_PhoneNumber();
$phone->setNumber("1234 5 67 79");
$phone->setType(Tutorial_Person_PhoneType::HOME);
$person->appendPhone($phone);

echo $person;
exit;
echo "____________________\n";

$addressbook->appendPerson($person);


var_dump($addressbook);

save($db, $addressbook);

exit;

function load($path)
{
    $data = @file_get_contents($path);
    try {
        $result = ProtocolBuffers::decode("Tutorial_AddressBook", $data);
    } catch (ProtocolBuffersInvalidProtocolBufferException $e) {
        $result = new Tutorial_AddressBook();
    }

    return $result;
}

function save($path, Tutorial_AddressBook $addresbook)
{
    file_put_contents($path, $addresbook->serializeToString());
}
