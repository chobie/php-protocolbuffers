#include "protoc-gen-php.h"

int main(int argc, char* argv[]) {
	google::protobuf::compiler::php::PHPCodeGenerator generator;

	return PluginMain(argc, argv, &generator);
}