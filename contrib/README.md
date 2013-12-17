protoc-gen-php: PSR-4 style generator
=====================================

protoc php generator plugin for php-protocolbuffers.

## Dependencies

* Cmake 2.8 higher
* Protocol Buffers 2.5 higher

## Compile and install

to compile protoc-gen-php

````
mkdir build && cd build && cmake ..
cmake --build .
````

to install the protoc-gen-php and php_options.proto system-wide.

````
sudo cmake --build . --target install
````

## Usage

````
protoc --php_out=<path_to_output> -I.  <path_to .proto>

````

NOTE: please remove old files when you re-generate messages.

## Options

You can control `protoc-gen-php` behaviour. please also check `exmaples/addressbook.proto`

PHPMessageOptions

| Option                   |  Type       |  Default   | Description                   |
|:------------------------ |:-----------:|:----------:|:------------------------------|
| `use_single_property`    | boolean     |  `false`   | bundle each fileds to single property |
| `single_property_name`   | string      |  `false`   | single property name |
| `use_wakeup_and_sleep`   | boolean     |  `false`   | enable `__wakeup` and `__sleep` serialization hooks |

PHPFileOptions

| Option                   |  Type       |  Default   | Description                   |
|:------------------------ |:-----------:|:----------:|:------------------------------|
| `generate_package`       | boolean     |  `true`    | use package name as namespace |
| `multiple_files`         | boolean     |  `true`    | output as individual php files |
| `skip_unknown`           | boolean     |  `false`   | parse and hold unknown fileds |
| `base_class`             | string      |  ``        | set parent class name. default is `ProtocolBuffersMessage` |

## License

new BSD License