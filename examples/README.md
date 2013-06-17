# Protocol Buffers Examples

````
# generate message classes from .proto file
protoc --plugin=../contrib/protoc-gen-php addressbook.proto --php_out=php -I . -I /usr/local/include
````
