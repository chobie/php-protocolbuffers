# Protocol Buffer Example

Overview
--------

this example explains memcached like server which using protocol buffers.

Requirements
------------

* php 5.3 higher
* php-uv (https://github.com/chobie/php-uv/)


Generate messages
-----------------

````
protoc --plugin=../../contrib/protoc-gen-php message.proto --php_out=proto -I . -I /usr/local/include
````

Starting Server Process
-----------------------

```
php server.php
````

you need to run server process when playing this example.


SET operation
-------------

````
// php client.php SET <KEY> <VALUE>
php client.php SET chobie "Helo World"
````

GET operation
-------------

````
// php client.php GET <KEY>
php client.php GET chobie
````
