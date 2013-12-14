#ifndef GOOGLE_PROTOBUF_COMPILER_PHP_FILE_H__
#define GOOGLE_PROTOBUF_COMPILER_PHP_FILE_H__

#include <string>
#include <vector>
#include <google/protobuf/stubs/common.h>

namespace google {
namespace protobuf {
  class FileDescriptor;        // descriptor.h
  namespace io {
    class Printer;             // printer.h
  }
  namespace compiler {
    class GeneratorContext;     // code_generator.h
  }
}

namespace protobuf {
namespace compiler {
namespace php {

class FileGenerator {
    public:
    explicit FileGenerator(const FileDescriptor* file);
    ~FileGenerator();

    bool Validate(string* error);

    void Generate(io::Printer* printer);

    // If we aren't putting everything into one file, this will write all the
    // files other than the outer file (i.e. one for each message, enum, and
    // service type).
    void GenerateSiblings(const string& package_dir,
                        GeneratorContext* generator_context,
                        vector<string>* file_list);

    private:
        const FileDescriptor *file_;
};

}  // namespace php
}  // namespace compiler
}  // namespace protobuf

}  // namespace google
#endif  // GOOGLE_PROTOBUF_COMPILER_PHP_FILE_H__
