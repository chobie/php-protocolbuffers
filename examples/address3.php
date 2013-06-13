<?php
require dirname(dirname(__FILE__)) . DIRECTORY_SEPARATOR . "contrib/php/addressbook.proto.php";

$person = new Tutorial_Person();
$person->setId(21);
$person->setName("John Doe");

$data = $person->serializeToString($person);

printf("# decoding benchmark (protocolbuffers).\n");
printf("# target message size: %d bytes.\n", strlen($data));
printf("# start\n");

$begin = microtime(true);
$attempts = 100000;
for ($i = 0; $i < $attempts; $i++) {
    Tutorial_Person::parseFromString($data);
}
$end = microtime(true);

printf("# Result:\n");
printf("# decoding %d messages\n", $attempts);
printf("# processing time: %6f (%f/message)\n", $end - $begin, ($end - $begin) / $attempts);
printf("# total bytes: %d bytes\n", strlen($data) * $attempts);
