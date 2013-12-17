// Copyright 2013 Shuhei Tanuma.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <google/protobuf/compiler/php/php_generator.h>
#include <google/protobuf/compiler/php/php_file.h>
#include <google/protobuf/compiler/php/php_enum.h>
#include <google/protobuf/compiler/php/php_message.h>
#include <google/protobuf/compiler/php/php_helpers.h>

#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace php {

FileGenerator::FileGenerator(const FileDescriptor* file,
    GeneratorContext *context)
    : file_(file),
  php_package_(FilePhpPackage(file)),
  context_(context) {
}


FileGenerator::~FileGenerator() {
}

bool FileGenerator::Validate(string* error) {
  // TODO(chobie): Validate proto file
  return true;
}


template<typename DescriptorClass>
string FileGenerator::NameSpace(const DescriptorClass* descriptor) const {
  vector<string> result;
  string output;
  string a = descriptor->package();

  if (!a.empty()) {
    SplitStringUsing(a, ".", &result);
    result.pop_back();
    JoinStrings(result,
    "\\",
    &output);
  } else {
    output = "";
  }

  return output;
}

template<typename DescriptorClass>
bool FileGenerator::HasNameSpace(const DescriptorClass* descriptor) const {
  string name = NameSpace(descriptor);

  if (!name.empty()) {
    return true;
  } else {
    return false;
  }
}

template<typename GeneratorClass, typename DescriptorClass>
static void GenerateSibling(const string& package_dir,
                            const string& java_package,
                            const DescriptorClass* descriptor,
                            GeneratorContext* context,
                            vector<string>* file_list,
                            const string& name_suffix,
                            void (GeneratorClass::*pfn)(io::Printer* printer)) {
  string filename = package_dir + descriptor->name() + name_suffix + ".php";
  file_list->push_back(filename);

  scoped_ptr<io::ZeroCopyOutputStream> output(context->Open(filename));
  io::Printer printer(output.get(), '`');

  GeneratorClass generator(descriptor, context, file_list);
  (generator.*pfn)(&printer);
}

template<typename GeneratorClass, typename DescriptorClass>
static void GenerateSibling(const string& package_dir,
                            const string& java_package,
                            const DescriptorClass* descriptor,
                            io::Printer *printer,
                            GeneratorContext* context,
                            vector<string>* file_list,
                            const string& name_suffix,
                            void (GeneratorClass::*pfn)(io::Printer* printer)) {

  GeneratorClass generator(descriptor, context, file_list);
  (generator.*pfn)(printer);
}


void FileGenerator::Generate(io::Printer* printer) {
  printer->Print("<?php\n");

  if (!file_->options().GetExtension(::php).multiple_files()) {
    vector<string> file_list;

    for (int i = 0; i < file_->enum_type_count(); i++) {
      GenerateSibling<EnumGenerator>("", "",
      file_->enum_type(i),
      printer, context_, &file_list, "",
      &EnumGenerator::Generate);
    }

    for (int i = 0; i < file_->message_type_count(); i++) {
      GenerateSibling<MessageGenerator>("", "",
      file_->message_type(i),
      printer, context_, &file_list, "",
      &MessageGenerator::Generate);
    }
  } else {
    printer->Print("require __DIR__ . DIRECTORY_SEPARATOR . 'autoload.php';\n");
  }
}

void FileGenerator::GenerateAutoloader(io::Printer *printer, vector<string> *file_list) {
  printer->Print("<?php\n");
  vector<string>::iterator x = file_list->begin();

  printer->Print("spl_autoload_register(function($name){\n"
    "    static $classmap;\n"
    "    if (!$classmap) {\n"
    "        $classmap = array(\n");
  while (x != file_list->end()) {
    string tmp(*x);
    replace(tmp.begin(), tmp.end(), '/', '\\');
    tmp = tmp.substr(0, tmp.find_last_of("."));

    printer->Print("            '`key`' => '`path`',\n",
      "key", tmp.c_str(),
      "path", (*x).c_str());
    ++x;
  }

  printer->Print("        );\n    }\n"
    "    if (isset($classmap[$name])) {\n"
    "        require __DIR__ . DIRECTORY_SEPARATOR . $classmap[$name];\n"
    "    }\n"
    "});\n");


}


void FileGenerator::GenerateSiblings(const string& package_dir,
                                     GeneratorContext* context,
                                     vector<string>* file_list) {
  if (file_->options().GetExtension(::php).multiple_files()) {
    for (int i = 0; i < file_->enum_type_count(); i++) {
      GenerateSibling<EnumGenerator>(package_dir, "",
      file_->enum_type(i),
      context, file_list, "",
      &EnumGenerator::Generate);
    }

    for (int i = 0; i < file_->message_type_count(); i++) {
      GenerateSibling<MessageGenerator>(package_dir, "",
      file_->message_type(i),
      context, file_list, "",
      &MessageGenerator::Generate);
    }
// TODO(chobie): implement service
//    if (HasGenericServices(file_)) {
//      for (int i = 0; i < file_->service_count(); i++) {
//        GenerateSibling<ServiceGenerator>(package_dir, java_package_,
//                                          file_->service(i),
//                                          context, file_list, "",
//                                          &ServiceGenerator::Generate);
//      }
//    }
  }
}

}  // namespace php
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
