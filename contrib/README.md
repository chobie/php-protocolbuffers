protoc-gen-php: PSR-4 style generator
=====================================

## Install

````
mkdir build
cd build
cmake ..
cmake --build .
cmake --build . --target install
````

## Requirements

Protocol Buffers 2.5 higher

## Usage

````
protoc --php_out=<path_to_output> -I.  <path_to .proto>

````

NOTE: please remove old files when you re-generate messages.


## License

new BSD License