// Copyright 2013 Shuhei Tanuma.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef GOOGLE_PROTOBUF_COMPILER_PHP_PHP_HELPERS_H_
#define GOOGLE_PROTOBUF_COMPILER_PHP_PHP_HELPERS_H_

#include "strutil.h"

#include <string>

#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/descriptor.h>

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

#endif  // GOOGLE_PROTOBUF_COMPILER_PHP_PHP_HELPERS_H_

