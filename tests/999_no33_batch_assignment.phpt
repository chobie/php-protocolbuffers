--TEST--
#33 fix batch assignment
--FILE--
<?php
require "messages" . DIRECTORY_SEPARATOR . "outer_inner.proto.php";

$message = new OuterMessage(array(
    "messages" => array(
        array(
            "id" => 1,
            "name" => "John",
        ),
        array(
            "id" => 2,
            "name" => "Belushi",
        )
    ),
    "message" => array(
        "id" => 3,
        "name" => "Adam",
    ),
));

var_dump($message);
--EXPECT--
object(OuterMessage)#1 (2) {
  ["messages":protected]=>
  array(2) {
    [0]=>
    object(OuterMessage_InnerMessage)#2 (2) {
      ["id":protected]=>
      int(1)
      ["name":protected]=>
      string(4) "John"
    }
    [1]=>
    object(OuterMessage_InnerMessage)#3 (2) {
      ["id":protected]=>
      int(2)
      ["name":protected]=>
      string(7) "Belushi"
    }
  }
  ["message":protected]=>
  object(OuterMessage_InnerMessage)#5 (2) {
    ["id":protected]=>
    int(3)
    ["name":protected]=>
    string(4) "Adam"
  }
}