<?php
require dirname(dirname(dirname(__FILE__))) . DIRECTORY_SEPARATOR . "contrib/php/addressbook.proto.php";

$person = new Tutorial_Person();
$person->setId(21);
$person->setName("John Doe");

$descriptor = $person->getDescriptor();
foreach ($descriptor->getFields() as $field) {
/*
 * $result = array();
 * $key = $field->getName();
 * $has_method = "has$key";
 * $get_method = "get$key";
 *
 * if ($person->$has_method()) {
 *   $value = $person->$get_method();
 *   switch ($field->getType()) {
 *       case ProtocolBuffers::WIRETYPE_VARINT:
 *         $result[$field->getName()] = $value;
 *       break;
 *       ...
 *   }
 * }
 * }
 *
 * json_encode($result);
 *
 */
}
