#include "protoc-gen-php.h"

int main(int argc, char* argv[]) {
	PHPCodeGenerator generator;

	return PluginMain(argc, argv, &generator);
}