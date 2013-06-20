<?php
require dirname(dirname(dirname(__FILE__))) . DIRECTORY_SEPARATOR . "contrib/php/addressbook.proto.php";

$data = msgpack_pack(array("id" => 21, "name" => "John Doe"));

printf("# decoding benchmark (msgpack array).\n");
printf("# target message size: %d bytes.\n", strlen($data));
printf("# start\n");

$begin = microtime(true);
$attempts = 100000;
for ($i = 0; $i < $attempts; $i++) {
    msgpack_unpack($data);
}
$end = microtime(true);

printf("# Result:\n");
printf("# decoding %d messages\n", $attempts);
printf("# processing time: %6f (%f/message)\n", $end - $begin, ($end - $begin) / $attempts);
printf("# total bytes: %d bytes\n", strlen($data) * $attempts);
