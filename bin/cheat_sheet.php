<?php

$wires = array(
  "0" => "varint",
  "1" => "64bit",
  "2" => "length delimited",
  "3" => "deprecated",
  "4" => "deprecated",
  "5" => "32bit",
);

// for reading manually
for ($x = 1; $x < 16; $x++) {
  printf("== %d ==\n", $x);
  for ($i = 0; $i < 6; $i++) {
    printf("  %02x => %s\n", (($x << 3) | ($i)), $wires[$i]);
  }
}
