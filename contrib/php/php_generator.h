// Copyright 2013 Shuhei Tanuma.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef PHP_PROTOC_GEN_INCLUDED
#define PHP_PROTOC_GEN_INCLUDED

#include <map>
#include <string>
#include <algorithm>
#include <vector>

#include <cstdio>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/wire_format_lite_inl.h>


#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/code_generator.h>

#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include "php_file.h"
#include "php_options.pb.h"

namespace google {
namespace protobuf {
namespace compiler {
namespace php {

class LIBPROTOC_EXPORT PHPGenerator : public CodeGenerator{
    public:
        PHPGenerator();
        ~PHPGenerator();

        // implements CodeGenerator ----------------------------------------
        bool Generate(const FileDescriptor* file,
                    const string& parameter,
                    GeneratorContext* context,
                    string* error) const;

    private:
};

}  // namespace php
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
#endif