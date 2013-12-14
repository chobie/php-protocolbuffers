#include "php_generator.h"
#include "php_enum.h"

namespace google {
namespace protobuf {
namespace compiler {
namespace php {

EnumGenerator::EnumGenerator(const EnumDescriptor* descriptor, GeneratorContext* context)
    : descriptor_(descriptor),
      context_(context)
{
}

EnumGenerator::~EnumGenerator()
{
}


void EnumGenerator::Generate(io::Printer* printer)
{
}

}  // namespace php
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
