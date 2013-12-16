// Copyright 2013 Shuhei Tanuma.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "google/protobuf/compiler/php/php_generator.h"

#include <iostream>

int main(int argc, char* argv[]) {
    google::protobuf::compiler::php::PHPGenerator generator;

    std::cerr << "protoc-gen-php 0.1.0 by chobie\n" << std::endl;

    return PluginMain(argc, argv, &generator);
}
