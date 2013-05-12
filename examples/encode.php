<?php
require join(DIRECTORY_SEPARATOR, array(dirname(dirname(__FILE__)), "src","ProtocolBuffers/Support.php"));
require dirname(dirname(__FILE__)) . DIRECTORY_SEPARATOR . "tests/Message/Base.php";
require dirname(dirname(__FILE__)) . DIRECTORY_SEPARATOR . "tests/Message/User.php";
require dirname(dirname(__FILE__)) . DIRECTORY_SEPARATOR . "tests/Message/Addressbook.php";
require dirname(dirname(__FILE__)) . DIRECTORY_SEPARATOR . "contrib/php/addressbook.proto.php";

$person = new Tutorial_Person();
$person->_properties['id']  = 21;
$person->_properties['name']  = "John Doe";

//$begin = microtime(true);
//for ($i = 0; $i < 10000; $i++) {
    $data = ProtocolBuffers::encode($person);
//}
//$end = microtime(true);

//var_dump(ProtocolBuffers::decode($person->getDescriptor(), "Tutorial_Person", $data));

$u = new User();
$u->setId(20);
$u->setOffense(105);
$u->setName("chobie");

$data = ProtocolBuffers::encode($u);
echo $data;
//var_dump(ProtocolBuffers::decode($u->getDescriptor(), "User", $data));