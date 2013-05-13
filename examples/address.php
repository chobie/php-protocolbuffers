<?php
require join(DIRECTORY_SEPARATOR, array(dirname(dirname(__FILE__)), "src","ProtocolBuffers/Support.php"));
require dirname(dirname(__FILE__)) . DIRECTORY_SEPARATOR . "ext/tests/messages/Base.php";
require dirname(dirname(__FILE__)) . DIRECTORY_SEPARATOR . "ext/tests/messages/User.php";
require dirname(dirname(__FILE__)) . DIRECTORY_SEPARATOR . "ext/tests/messages/Addressbook.php";
require dirname(dirname(__FILE__)) . DIRECTORY_SEPARATOR . "contrib/php/addressbook.proto.php";

$person = new tutorial_Person();
$person->_properties['id']  = 21;
$person->_properties['name']  = "John Doe";

$data = ProtocolBuffers::encode($person);
var_dump(ProtocolBuffers::decode(Tutorial_Person::getDescriptor(), "tutorial_Person", $data));
//exit;


$u = new User();
$u->setId(20);
$u->setOffense(105);
$u->setName("chobie");

$u2 = new User();
$u2->setId(21);
$u2->setOffense(108);

$u2->setName("charmy");

$addr = new AddressBook();
$addr->addUser($u);
$addr->addUser($u2);

$data = ProtocolBuffers::encode($u2);
//echo $data;

//$data = ProtocolBuffers::encode($addr->getDescriptor(), $addr);
$begin = microtime(true);
$proto = $u2->getDescriptor();
$d = serialize($u2);
for ($i = 0; $i < 10000; $i++) {
    ProtocolBuffers::decode($proto, "User", $data);
    //unserialize($d);
}
$end = microtime(true);
var_dump($end - $begin);

//var_dump(AddressBook::unserialize($data));
//echo ProtocolBuffers::generateProto("User");
//echo ProtocolBuffers::generateProto("AddressBook");
