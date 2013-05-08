ProtocolBuffers decoder
=======================

Requirements
------------

Message class have to implement `array getProto()` static method and returns array have to be...

````
array(
  "<tag_id>" => array(
    "name"     => "<property_name>",
    "wiretype" => "<wiretype>",
    "type"     => "<type>",
    "message"  => "<message_name>",
    "required" => boolean,
    "repeated" => boolean,
  ),
  ...
);
````


Functions
---------

* mixed pb_decode(array proto, string class_name, string serialized_string)