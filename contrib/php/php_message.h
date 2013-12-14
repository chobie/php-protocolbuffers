#ifndef GOOGLE_PROTOBUF_COMPILER_PHP_MESSAGE_H__
#define GOOGLE_PROTOBUF_COMPILER_PHP_MESSAGE_H__

#include <string>
#include <vector>
#include <google/protobuf/descriptor.h>

namespace google {
namespace protobuf {
  namespace io {
    class Printer;             // printer.h
  }
}

namespace protobuf {
namespace compiler {
namespace php {

class MessageGenerator {
    public:
    explicit MessageGenerator(const Descriptor* descriptor, GeneratorContext* context);
    ~MessageGenerator();

    void Generate(io::Printer* printer);
    private:
        string ClassName();
        string ClassName(const Descriptor &descriptor) const;
        string ClassName(const EnumDescriptor &descriptor) const;
        string NameSpace();
        bool HasNameSpace();
        string FileName();
        void PrintUseNameSpaceIfNeeded(io::Printer* printer);
        void PrintMemberProperties(io::Printer* printer);
        string getTypeName(const FieldDescriptor & field) const;
        void PrintGetDescriptor(io::Printer* printer);
        string VariableName(const FieldDescriptor & field) const;
        string DefaultValueAsString(const FieldDescriptor & field, bool quote_string_type) const;

        const Descriptor *descriptor_;
        GeneratorContext* context_;
};

}  // namespace java
}  // namespace compiler
}  // namespace protobuf

}  // namespace google
#endif  // GOOGLE_PROTOBUF_COMPILER_PHP_MESSAGE_H__
