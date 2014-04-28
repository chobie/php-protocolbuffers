<?php
require dirname(__DIR__) . DIRECTORY_SEPARATOR . "bootstrap.php";

$user = new pecl\protocolbuffers\User();
$user->setUserId(12345);
$user->setName("chobie");
echo $user->serializeToString();