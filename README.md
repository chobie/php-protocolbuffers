Protocol Buffers Extension for PHP
==================================

[![Build Status](https://secure.travis-ci.org/chobie/php-protocolbuffers.png)](http://travis-ci.org/chobie/php-protocolbuffers)

Motivation
----------

there are several protocol buffers implementation on php. but PHP really poor about decoding pb messeage.
especially reading `varint` is the bottleneck as it have to be call `ord` function each bytes.
(PHP's calling function is one of big cost. but we can't convert byte to int without `ord` or some function. as far as i known)

this library will provide really fast decoding / encoding pb message and also provides generator for .proto file (see contrib directory)

Status
------

Alpha

License
-------

BSD License

Requirements
------------

PHP5.3 higher

Install for developers
----------------------

    git clone https://github.com/chobie/php-protocolbuffers.git
    cd php-protocolbuffers
    phpize
    ./configure
    make
    make install
    # please add following line to your php.ini
    # extension=protocolbuffers.so

Building protoc-gen-php
-----------------------

currently, protoc-php-gen does not set PATH. you have to put it on to PATH manually

    cd php-protocolbuffers/contrib
    make

* you need to install protocol buffers (https://code.google.com/p/protobuf/)  before make protoc-php-gen.


Protocol Buffer Basics: PHP
---------------------------

This tutorial provides a basic PHP programmer's introduction to working with protocol buffers. By walking through creating a simple example application, it shows you how to

* Define message formats in a .proto file.
* Use the protocol buffer compiler.
* Use the PHP protocol buffer API to write and read messages.

Why Use Protocol Buffers?
-------------------------

The example we're going to use is a very simple "address book" application that can read and write people's contact details to and from a file. Each person in the address book has a name, an ID, an email address, and a contact phone number.

How do you serialize and retrieve structured data like this? There are a few ways to solve this problem:

* Use Database. This is the default approach since it's well documented.

* Use PHP Serialization. also this is very useful, but it doesn't work very well if you need to share data with applications written in other languages.

* Serialize the data to XML. This approach can be very attractive since XML is (sort of) human readable and there are binding libraries for lots of languages. This can be a good choice if you want to share data with other applications/projects. However, XML is notoriously space intensive, and encoding/decoding it can impose a huge performance penalty on applications. Also, navigating an XML DOM tree is considerably more complicated than navigating simple fields in a class normally would be.

Protocol buffers are the flexible, efficient, automated solution to solve exactly this problem. With protocol buffers, you write a .proto description of the data structure you wish to store. From that, the protocol buffer compiler creates a class that implements automatic encoding and parsing of the protocol buffer data with an efficient binary format. The generated class provides getters and setters for the fields that make up a protocol buffer and takes care of the details of reading and writing the protocol buffer as a unit. Importantly, the protocol buffer format supports the idea of extending the format over time in such a way that the code can still read data encoded with the old format.

Where to Find the Example Code
------------------------------

TBD

The Protocol Buffer API
-----------------------

TBD

Parsing and Serialization
-------------------------

TBD

Writing A Message
-----------------

````
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "contrib/php/addressbook.proto.php";

$person = new Tutorial_Person();
$person->setId(21);
$person->setName("John Doe");

$data = $person->serializeToString($person);
````

Advanced Usage
--------------

TBD

Compatibility
-------------

<table>
  <tr>
    <td>repeated fields</td><td>supported</td>
  </tr>
  <tr>
    <td>packed attributes</td><td>supported</td>
  </tr>
  <tr>
    <td>extensions</td><td>limited(only php message options) supported (fully feature will be add in July, 2013)</td>
  </tr>
  <tr>
    <td>unknown fields</td><td>limited supported (decoding only and don't care repeated fields)</td>
  </tr>
  <tr>
    <td>Service (RPC)</td><td>not supported yet</td>
  </tr>
</table>

Basically, php-protocolbuffers test cases were made by python generator.
at least it proves correct behavior in those test cases.

Thanks
-------

- Google Protocol Buffers team.

pb is very interesting. thanks providing it!

- bramp's protoc-gen-php

https://github.com/bramp/protoc-gen-php

my protoc-gen-php based on his work.
