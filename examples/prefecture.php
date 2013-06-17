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
