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

foreach ($message->getMessages() as $inner) {
	echo $inner->getId() . PHP_EOL;
	echo $inner->getName() . PHP_EOL;
}
$adam = $message->getMessage();
echo $adam->getId() . PHP_EOL;
echo $adam->getName() . PHP_EOL;

--EXPECT--
1
John
2
Belushi
3
Adam