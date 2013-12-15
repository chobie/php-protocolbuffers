// Copyright 2013 Shuhei Tanuma.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "php/php_generator.h"

int main(int argc, char* argv[]) {
    google::protobuf::compiler::php::PHPGenerator generator;

    return PluginMain(argc, argv, &generator);
}
