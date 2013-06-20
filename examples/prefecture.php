<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . join(DIRECTORY_SEPARATOR, array("php", "prefecture.proto.php"));

$a = file_get_contents("prefecture.tsv");

$jpn = new Tutorial_Japan();
$i = 1;
foreach (explode("\n", $a) as $line) {
    $line = trim($line);
    if (empty($line)) {
        continue;
    }

    $args = explode("\t", trim($line));
    $prefecture = new Tutorial_Prefecture();
    $prefecture->setId($i);
    $prefecture->setName($args[0]);
    $prefecture->setPopulation($args[1]);
    $prefecture->setArea($args[2]);
    $prefecture->setPopulationDensity($args[3]);

    $jpn->appendPrefecture($prefecture);
    $i++;
}

file_put_contents("prefecture.db", $jpn->serializeToString());
$data = $jpn->serializeToString();

$data = file_get_contents("prefecture.db");
printf("# decoding benchmark (protocolbuffers).\n");
printf("# target message size: %d bytes.\n", strlen($data));
printf("# start\n");

$begin = microtime(true);
$attempts = 10000;
$b = memory_get_usage();
for ($i = 0; $i < $attempts; $i++) {
    $a = ProtocolBuffers::decode("Tutorial_Japan", $data);
}
$e = memory_get_usage();
var_dump(($e - $b) / 1024);
exit;
$end = microtime(true);

printf("# Result:\n");
printf("# decoding %d messages\n", $attempts);
printf("# processing time: %6f (%f/message)\n", $end - $begin, ($end - $begin) / $attempts);
printf("# total bytes: %d bytes\n", strlen($data) * $attempts);
