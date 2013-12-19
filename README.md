Protocol Buffers Extension for PHP
==================================

[![Build Status](https://secure.travis-ci.org/chobie/php-protocolbuffers.png)](http://travis-ci.org/chobie/php-protocolbuffers)

## Status

Alpha

## License

New BSD License

## Requirements

* PHP5.3 higher

PHP5.2 is limited support (can't use `ProtocolBuffers\Message::parseFromString()`. you have to encode / decode via ProtocolBuffers methods).

* Google's `protoc` compiler version 2.3 or above ¹

 ¹Only needed for generate classes from `.proto` file.


## Install for developers

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

## Building protoc-gen-php

* you need to install protocol buffers (https://code.google.com/p/protobuf/)  before make protoc-php-gen.


````
    cd php-protocolbuffers/contrib
    mkdir build
    cd build
    cmake ..
    make
    # install protoc-gen-php to /usr/local/bin
    sudo make install
````


### The Protocol Buffer API

basically, your message classes should inherit ProtocolBuffersMessage class.

`ProtocolBuffers\Message` implements `ProtocolBuffers\Describable`, `Iterator`

````
ProtocolBuffers\Message::parseFromString($bytes);
ProtocolBuffers\Message->serializeToString();
ProtocolBuffers\Message->mergeFrom((SameMessageClass) $message);
ProtocolBuffers\Message->clear($name);
ProtocolBuffers\Message->clearAll($clear_unknown_fields = true);
ProtocolBuffers\Message->getUnknownFieldSet();
ProtocolBuffers\Message->discardUnknownFields();
ProtocolBuffers\Message->get($name);
ProtocolBuffers\Message->has($name);
ProtocolBuffers\Message->set($name, $value);
ProtocolBuffers\Message->setFrom(array $values);
ProtocolBuffers\Message->getExtension($name);
ProtocolBuffers\Message->hasExtension($name);
ProtocolBuffers\Message->setExtension($name, $value);

# magic methods.
ProtocolBuffers\Message->get<CamelCaseFieldName>();
ProtocolBuffers\Message->set<CamelCaseFieldName>($value);
ProtocolBuffers\Message->append<CamelCaseFieldNameForRepeatedField>($value);
ProtocolBuffers\Message->clear<CamelCaseFieldName>();
ProtocolBuffers\Message->has<CamelCaseFieldName>();
````

### Writing A Message

````php
<?php
require dirname(__FILE__) . DIRECTORY_SEPARATOR . "contrib/php/addressbook.proto.php";

$person = new Tutorial_Person();
$person->setId(21);
$person->setName("John Doe");

$data = $person->serializeToString($person);
````
### Advanced Usage

PHP Protocol Buffers implementation also provides `ProtocolBuffers::encode(ProtocolBuffersDescribable $message)` and `ProtocolBuffers::decode(string classname, $bytes)` methods.
you can serialize / deserialize any object which implements `ProtocolBuffers\Describable` or "public static getDescriptor()" method. this is very useful when
evaluating protocol buffers serialization.

### Scalar Value Types

| .proto type | encoding       | php type     |
|:------------|---------------:|:------------:|
| double      |                | string       |
| float       |                | string       |
| int32       | varint         | string       |
| int64       | varint         | string       |
| uint32      | varint         | string       |
| uint64      | varint         | string       |
| sint32      | zigzag+varint  | string       |
| sint64      | zigzag+varint  | string       |
| fixed32     |                | string       |
| fixed64     |                | string       |
| sfixed32    |                | string       |
| sfixed64    |                | string       |
| bool        |                | bool         |
| string      | `utf8` string  | string       |
| bytes       |                | string       |

Note: PHP's numeric value is platform dependant. Unfortunately long is 32bit value on some 64bit platform.
It's impossible to keep precision for many types with PHP. so we use string types. it's safe.
Don't forget. You have to use GMP or BCMath for calculating values which exceeds PHP_INT_MAX.

maybe I'll re-support long values for performance reason. but it can't handle big values correctly.

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
    <td>Extensions</td><td>alpha</td>
  </tr>
  <tr>
    <td>Unknown fields</td><td>beta</td>
  </tr>
  <tr>
    <td>Service (RPC)</td><td>not supported yet</td>
  </tr>
  <tr>
    <td>64bit values</td><td>EXPERIMENTAL</td>
  </tr>
</table>

## PHP specific features

* `__sleep` and `__wakeup` method support.

You can use above magic methods when enabling UseWakeupAndSleep flag. Be careful, this feature is not part of protocolbuffers specification.
these method will call when `parseFromString` and `serializeToString` (also supports `ProtocolBuffers::decode` and `ProtocolBuffers::encode`)


````
            $phpoptions = $descriptor_builder->getOptions()->getExtension(ProtocolBuffers::PHP_MESSAGE_OPTION);
            $phpoptions->setUseWakeupAndSleep(true); // default is off
            $descriptor = $descriptor_builder->build();
````

NOTE: We do not support `Serializable` interface.

## Thanks

- Google Protocol Buffers team.

pb is very interesting. thanks providing it!

- bramp's protoc-gen-php

https://github.com/bramp/protoc-gen-php

my protoc-gen-php based on his work.


## Windows Support

You need to build your self. please check https://wiki.php.net/internals/windows/stepbystepbuild

````
phpize
configure --with-protocolbuffers
nmake
phpunit
````

## FAQ

#### how can i delete item with repeated field?

Basically, protobuf doesn't provide API which arbitrarily remove an element from within a repeated field.
You should define a helper method on helper class and use it. someday I'll add helper class for generic usage.


#### can we implement business logic on message class?

We strongly disagree with this opinion. the message class is just container. You should wrap the message class or
add some business logic class which modify message from outside.

first, please check [Google's tutorial](https://developers.google.com/protocol-buffers/docs/cpptutorial)