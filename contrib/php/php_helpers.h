#ifndef GOOGLE_PROTOBUF_COMPILER_PHP_HELPERS_H__
#define GOOGLE_PROTOBUF_COMPILER_PHP_HELPERS_H__

#include <string>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/descriptor.h>
#include "strutil.h"

namespace google {
namespace protobuf {
namespace compiler {
namespace php {

string FilePhpPackage(const FileDescriptor* file);

string PhpPackageToDir(string package_name);

string UnderscoresToCamelCaseImpl(const string& input, bool cap_next_letter);

string UnderscoresToCamelCase(const FieldDescriptor& field);

string UnderscoresToCapitalizedCamelCase(const FieldDescriptor& field);

string LowerString(const string & s);

string UpperString(const string & s);

}  // namespace php
}  // namespace compiler
}  // namespace protobuf

}  // namespace google
#endif  // GOOGLE_PROTOBUF_COMPILER_PHP_HELPERS_H__
