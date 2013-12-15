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

## Usage

````
protoc --php_out=<path_to_output> -I.  <path_to .proto>

````

NOTE: please remove old files when you re-generate with protoc.


## License

new BSD License