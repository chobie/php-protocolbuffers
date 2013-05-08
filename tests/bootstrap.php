<?php
require join(DIRECTORY_SEPARATOR, array(dirname(dirname(__FILE__)), "src","ProtocolBuffers.php"));
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "Message/Base.php";
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "Message/Addressbook.php";
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "Message/User.php";

define("FIXTURE_PATH", dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures");
