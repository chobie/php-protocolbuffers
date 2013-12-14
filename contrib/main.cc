#include "php/php_generator.h"

int main(int argc, char* argv[]) {
    google::protobuf::compiler::php::PHPGenerator generator;

    return PluginMain(argc, argv, &generator);
}