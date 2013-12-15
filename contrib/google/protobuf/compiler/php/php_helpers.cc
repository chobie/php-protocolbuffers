// Copyright 2013 Shuhei Tanuma.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <google/protobuf/compiler/php/php_helpers.h>

#include <google/protobuf/descriptor.pb.h>

#include <limits>
#include <vector>

namespace google {
namespace protobuf {
namespace compiler {
namespace php {

char *kDefaultPackage = "";

string FilePhpPackage(const FileDescriptor* file) {
  string result;

  if (file->options().has_java_package()) {
    result = file->options().java_package();
  } else {
    result = kDefaultPackage;
    if (!file->package().empty()) {
      if (!result.empty()) result += '.';
      result += file->package();
    }
  }


  return result;
}


string PhpPackageToDir(string package_name) {
  string package_dir =
    StringReplace(package_name, ".", "/", true);
  if (!package_dir.empty()) package_dir += "/";
  return package_dir;
}

string UnderscoresToCamelCaseImpl(const string& input, bool cap_next_letter) {
    string result;
    // Note:  I distrust ctype.h due to locales.
    for (int i = 0; i < static_cast<int>(input.size()); i++) {
        if ('a' <= input[i] && input[i] <= 'z') {
            if (cap_next_letter) {
                result += input[i] + ('A' - 'a');
            } else {
                result += input[i];
            }
            cap_next_letter = false;
        } else if ('A' <= input[i] && input[i] <= 'Z') {
            if (i == 0 && !cap_next_letter) {
                // Force first letter to lower-case unless explicitly told to
                // capitalize it.
                result += input[i] + ('a' - 'A');
            } else {
                // Capital letters after the first are left as-is.
                result += input[i];
            }
            cap_next_letter = false;
        } else if ('0' <= input[i] && input[i] <= '9') {
            result += input[i];
            cap_next_letter = true;
        } else {
            cap_next_letter = true;
        }
    }
    return result;
}

string UnderscoresToCamelCase(const FieldDescriptor& field) {
    return UnderscoresToCamelCaseImpl(field.name(), false);
}

string UnderscoresToCapitalizedCamelCase(const FieldDescriptor& field) {
    return UnderscoresToCamelCaseImpl(field.name(), true);
}

string LowerString(const string & s) {
    string newS(s);

    google::protobuf::LowerString(&newS);
    return newS;
}

string UpperString(const string & s) {
    string newS(s);

    google::protobuf::UpperString(&newS);
    return newS;
}

}  // namespace php
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
