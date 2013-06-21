Protocol Buffers Extension for PHP
==================================

[![Build Status](https://secure.travis-ci.org/chobie/php-protocolbuffers.png)](http://travis-ci.org/chobie/php-protocolbuffers)

## Status

Alpha

## License

BSD License

## Requirements

* PHP5.3 higher

PHP5.2 is limited support (can't use `ProtocolBuffers\Message::parseFromString()`. you have to encode / decode via ProtocolBuffers methods).

* Google's `protoc` compiler version 2.3 or above ¹

 ¹Only needed for generate classes from `.proto` file.


## Install for developers

    git clone https://github.com/chobie/php-protocolbuffers.git
    cd php-protocolbuffers
    phpize
    ./configure
    make
    make install
    # please add following line to your php.ini
    # extension=protocolbuffers.so

## Building protoc-gen-php

currently, `protoc-php-gen` does not set PATH. you have to put it on to PATH manually

    cd php-protocolbuffers/contrib
    make

* you need to install protocol buffers (https://code.google.com/p/protobuf/)  before make protoc-php-gen.

## Protocol Buffer Basics: PHP

This tutorial provides a basic PHP programmer's introduction to working with protocol buffers. By walking through creating a simple example application, it shows you how to

* Define message formats in a .proto file.
* Use the protocol buffer compiler.
* Use the PHP protocol buffer API to write and read messages.

### Why Use Protocol Buffers?

The example we're going to use is a very simple "address book" application that can read and write people's contact details to and from a file. Each person in the address book has a name, an ID, an email address, and a contact phone number.

How do you serialize and retrieve structured data like this? There are a few ways to solve this problem:

* Use Database. This is the default approach since it's well documented. but it's hard to make a complicated tables. also consider scaling.

* Use PHP Serialization. also this is very useful, but it doesn't work very well if you need to share data with applications written in other languages.

* Serialize the data to XML. This approach can be very attractive since XML is (sort of) human readable and there are binding libraries for lots of languages. This can be a good choice if you want to share data with other applications/projects. However, XML is notoriously space intensive, and encoding/decoding it can impose a huge performance penalty on applications. Also, navigating an XML DOM tree is considerably more complicated than navigating simple fields in a class normally would be.

Protocol buffers are the flexible, efficient, automated solution to solve exactly this problem. With protocol buffers, you write a .proto description of the data structure you wish to store. From that, the protocol buffer compiler creates a class that implements automatic encoding and parsing of the protocol buffer data with an efficient binary format. The generated class provides getters and setters for the fields that make up a protocol buffer and takes care of the details of reading and writing the protocol buffer as a unit. Importantly, the protocol buffer format supports the idea of extending the format over time in such a way that the code can still read data encoded with the old format.

### Where to Find the Example Code

TBD

### The Protocol Buffer API

basically, your message classes should inherit ProtocolBuffersMessage class.

`ProtocolBuffers\Message` implements `ProtocolBuffers\Describable`, `Iterator`

````
ProtocolBuffers\Message::parseFromString($bytes);
ProtocolBuffers\Message->serializeToString();
ProtocolBuffers\Message->mergeFrom((SameMessageClass) $message);
ProtocolBuffers\Message->clear();
ProtocolBUffers\Message->discardUnknownFields();

# magic methods.
ProtocolBuffers\Message->set<CamelCaseFieldName>($value);
ProtocolBuffers\Message->append<CamelCaseFieldNameForRepeatedField>($value);
ProtocolBuffers\Message->clear<CamelCaseFieldName>();
````

(will be add getter / setter magic methods soon.)

### Writing A Message

````php
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "contrib/php/addressbook.proto.php";

$person = new Tutorial_Person();
$person->setId(21);
$person->setName("John Doe");

$data = $person->serializeToString($person);
````

### Extending a Protocol Buffer

Sooner or later after you release the code that uses your protocol buffer, you will undoubtedly want to "improve" the protocol buffer's definition. If you want your new buffers to be backwards-compatible, and your old buffers to be forward-compatible – and you almost certainly do want this – then there are some rules you need to follow. In the new version of the protocol buffer:

* you must not change the tag numbers of any existing fields.

* you must not add or delete any required fields.

* you may delete optional or repeated fields.

* you may add new optional or repeated fields but you must use fresh tag numbers (i.e. tag numbers that were never used in this protocol buffer, not even by deleted fields).


(There are some exceptions to these rules, but they are rarely used.)

If you follow these rules, old code will happily read new messages and simply ignore any new fields. To the old code, optional fields that were deleted will simply have their default value, and deleted repeated fields will be empty. New code will also transparently read old messages. However, keep in mind that new optional fields will not be present in old messages, so you will need to either check explicitly whether they're set with has_, or provide a reasonable default value in your .proto file with [default = value] after the tag number. If the default value is not specified for an optional element, a type-specific default value is used instead: for strings, the default value is the empty string. For booleans, the default value is false. For numeric types, the default value is zero. Note also that if you added a new repeated field, your new code will not be able to tell whether it was left empty (by new code) or never set at all (by old code) since there is no has_ flag for it.


### Advanced Usage

PHP Protocol Buffers implementation also provides `ProtocolBuffers::encode(ProtocolBuffersDescribable $message)` and `ProtocolBuffers::decode(string classname, $bytes)` methods.
you can serialize / deserialize any object which implements `ProtocolBuffers\Describable` or "public static getDescriptor()" method. this is very useful when
evaluating protocol buffers serialization.


### Features

<table>
  <tr>
    <td>Standard types (numbers, string, enums, messages, etc)</td><td>supported</td>
  </tr>
  <tr>
    <td>Repeated fields</td><td>supported</td>
  </tr>
  <tr>
    <td>Packed attributes</td><td>supported</td>
  </tr>
  <tr>
    <td>Extensions</td><td>limited(only php message options) supported (fully feature will be add in July, 2013)</td>
  </tr>
  <tr>
    <td>Unknown fields</td><td>limited supported (not fully tested. see test cases)</td>
  </tr>
  <tr>
    <td>Service (RPC)</td><td>not supported yet</td>
  </tr>
</table>

Basically, php-protocolbuffers test cases were made by python generator.
at least it proves correct behavior in those test cases.

## Thanks

- Google Protocol Buffers team.

pb is very interesting. thanks providing it!

- bramp's protoc-gen-php

https://github.com/bramp/protoc-gen-php

my protoc-gen-php based on his work.
