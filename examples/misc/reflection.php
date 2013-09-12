<?php
require dirname(dirname(dirname(__FILE__))) . DIRECTORY_SEPARATOR . "contrib/php/addressbook.proto.php";

// example: ProtocolBuffers to Json format

$person = new Tutorial_Person();
$person->setId(21);
$person->setName("John Doe");

$descriptor = $person->getDescriptor();
$result = array();

foreach ($descriptor->getFields() as $field) {
    if (!$field->isExtension()) {
        if ($person->has($field->getName())) {
            // this does not care about Message.
            $value = $person->get($field->getName());
            $result[$field->getName()] = $value;
        }
    }
}

echo json_encode($result, JSON_PRETTY_PRINT);
