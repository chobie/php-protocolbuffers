# php-protocolbuffers [![Build Status](https://secure.travis-ci.org/chobie/php-protocolbuffers.png)](http://travis-ci.org/chobie/php-protocolbuffers)

## Objective

php-protocolbuffers provides full protocol buffers features into PHP.

## Status

Alpha (considering phpish API design)

## Requirements

* PHP5.3 higher

PHP5.2 is limited support (can't use `ProtocolBuffers\Message::parseFromString()`. you have to encode / decode via ProtocolBuffers methods).

* Google's `protoc` compiler version 2.3 or above ¹

 ¹Only needed for generate classes from `.proto` file.


## Install

````
git clone https://github.com/chobie/php-protocolbuffers.git
cd php-protocolbuffers
phpize
./configure
make
make install
# please add following line to your php.ini
# extension=protocolbuffers.so
````

`protoc-gen-php` is related project (https://github.com/chobie/protoc-gen-php).
you need this when you generate message from .proto file.

### More information

see wiki https://github.com/chobie/php-protocolbuffers/wiki

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
    <td>Extensions</td><td>beta</td>
  </tr>
  <tr>
    <td>Unknown fields</td><td>beta</td>
  </tr>
  <tr>
    <td>64bit values</td><td>beta</td>
  </tr>
  <tr>
    <td>Service (RPC)</td><td>not supported yet</td>
  </tr>
</table>

## License

New BSD License

