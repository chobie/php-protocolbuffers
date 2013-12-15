// Copyright 2013 Shuhei Tanuma.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef GOOGLE_PROTOBUF_COMPILER_PHP_PHP_MESSAGE_H_
#define GOOGLE_PROTOBUF_COMPILER_PHP_PHP_MESSAGE_H_

#include <google/protobuf/descriptor.h>

#include <string>
#include <vector>

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
  MessageGenerator(const Descriptor* descriptor, GeneratorContext* context);
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
  void PrintMagicMethod(io::Printer* printer);
  void PrintMemberProperties(io::Printer* printer);
  string getTypeName(const FieldDescriptor & field) const;
  void PrintGetDescriptor(io::Printer* printer);
  string VariableName(const FieldDescriptor & field) const;
  string DefaultValueAsString(const FieldDescriptor &field,
    bool quote_string_type) const;
  string GetClassNameForFieldDescriptor();
  string GetClassNameForDescriptorBuilder();

  const Descriptor *descriptor_;
  GeneratorContext* context_;

  bool use_namespace_;
  bool enclose_namespace_;
};

}  // namespace php
}  // namespace compiler
}  // namespace protobuf

}  // namespace google
#endif  // GOOGLE_PROTOBUF_COMPILER_PHP_PHP_MESSAGE_H_
