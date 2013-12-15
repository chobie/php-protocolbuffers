// Copyright 2013 Shuhei Tanuma.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

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
    FileGenerator(const FileDescriptor* file, GeneratorContext *context);
    ~FileGenerator();

    bool Validate(string* error);

    void Generate(io::Printer* printer);

    // If we aren't putting everything into one file, this will write all the
    // files other than the outer file (i.e. one for each message, enum, and
    // service type).
    void GenerateSiblings(const string& package_dir,
                        GeneratorContext* generator_context,
                        vector<string>* file_list);

    string php_package()
    {
        return php_package_;
    }

    void GenerateAutoloader(io::Printer *printer);

    private:
        template<typename DescriptorClass>
        string NameSpace(const DescriptorClass* descriptor) const;

        template<typename DescriptorClass>
        bool HasNameSpace(const DescriptorClass* descriptor) const;

        const FileDescriptor *file_;
        string php_package_;
        GeneratorContext *context_;
};

}  // namespace php
}  // namespace compiler
}  // namespace protobuf

}  // namespace google
#endif  // GOOGLE_PROTOBUF_COMPILER_PHP_FILE_H__
