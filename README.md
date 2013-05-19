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

Under Development

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

currently, protoc-php-gen does not do install. you have to put it on to PATH manually

    cd php-protocolbuffers/contrib
    make

* you need to install protocol buffers (https://code.google.com/p/protobuf/)  before make protoc-php-gen.

Overview
--------

php-protocolbuffers provides ProtocolBuffers class.


### \ProtocolBuffers::decode(string $class_name, string $pb_bytes [, array $descriptor])

##### *Description*

decode protocol buffers with specified descriptor.

##### *Parameters*

*class_name*: top level class name of pb message.
*pb_bytes*: protocol buffers bytes
*descriptor*: optional. please pass a descriptor array specified class does not have getDescriptor method.

##### *Return Value*

*mixed*: specified class

##### *Example*


### \ProtocolBuffers::encode(object $object [, array $descriptor])

##### *Description*

encode object with specified descriptor.

##### *Parameters*

*object*: top level object.
*descriptor*: optional. please pass a descriptor array specified class does not have getDescriptor method.

##### *Return Value*

*string*: bytes

##### *Example*


Thanks
-------

- Google Protocol Buffers team.

pb is very interesting. thanks providing it!

- bramp's protoc-gen-php

https://github.com/bramp/protoc-gen-php

my protoc-gen-php based on his work.
