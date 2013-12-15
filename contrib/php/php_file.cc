#include "php_generator.h"
#include "php_file.h"
#include "php_enum.h"
#include "php_message.h"
#include "strutil.h"
#include "php_helpers.h"


#include <iostream>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace php {

FileGenerator::FileGenerator(const FileDescriptor* file, GeneratorContext *context)
    : file_(file),
      php_package_(FilePhpPackage(file)),
      context_(context)
{

}


FileGenerator::~FileGenerator()
{
}

bool FileGenerator::Validate(string* error) {
    return true;
}


template<typename DescriptorClass>
string FileGenerator::NameSpace(const DescriptorClass* descriptor) const
{
    vector<string> result;
    string output;

    SplitStringUsing(descriptor->file().name(), ".", &result);
    result.pop_back();

    JoinStrings(result,
                 "\\",
                 &output);

    return output;
}

template<typename DescriptorClass>
bool FileGenerator::HasNameSpace(const DescriptorClass* descriptor) const
{
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

    GeneratorClass generator(descriptor, context);
    (generator.*pfn)(&printer);

}


void FileGenerator::Generate(io::Printer* printer)
{
    if (!file_->options().GetExtension(::php).multiple_files()) {
        vector<string> *file_list;

        for (int i = 0; i < file_->enum_type_count(); i++) {
            GenerateSibling<EnumGenerator>("", "",
            file_->enum_type(i),
            context_, file_list, "",
            &EnumGenerator::Generate);
        }

        for (int i = 0; i < file_->message_type_count(); i++) {
            GenerateSibling<MessageGenerator>("", "",
            file_->message_type(i),
            context_, file_list, "",
            &MessageGenerator::Generate);
        }
    } else {
        GenerateAutoloader(printer);
    }
}

void FileGenerator::GenerateAutoloader(io::Printer *printer)
{
    printer->Print("<?php\n");
    printer->Print("namespace `namespace`;\n\n",
        "namespace",
        "google\\ad"
    );
    printer->Print(
        "/**\n"
        " * Example autoloader\n"
        " * \n"
        " * you should replace your own autoloader or composer's autoloader\n"
        " */\n"
    );
    printer->Print(
        "class Autoloader\n"
        "{\n"
    );
    printer->Indent();
    printer->Print(
        "const NAME_SPACE = \"google\\\\ad\";\n"
        "\n"
        "protected static $base_dir;\n"
        "\n"
    );
    printer->Print(
        "public static function register($dirname = null)\n"
        "{\n"
    );
    printer->Indent();
    printer->Print(
        "if (is_null($dirname)) {\n"
    );
    printer->Indent();
    printer->Print(
        "$dirname = dirname(__FILE__);\n"
    );
    printer->Outdent();
    printer->Print(
        "}\n"
        "self::$base_dir = $dirname;\n"
        "spl_autoload_register(array(__CLASS__, \"autoload\"));\n"
    );
    printer->Outdent();
    printer->Print(
        "}\n\n"
    );
    printer->Print(
        "public static function autoload($name)\n"
        "{\n"
    );
    printer->Indent();
    printer->Print(
        "$retval = false;\n"
        "if (strpos($name, self::NAME_SPACE) === 0) {\n"
    );
    printer->Indent();
    printer->Print(
        "if (strpos($name, self::NAME_SPACE) === 0) {\n"
    );
    printer->Indent();
    printer->Print(
        "$parts = explode(\"\\\\\", $name);\n"
        "$expected_path = join(DIRECTORY_SEPARATOR, array(self::$base_dir, join(DIRECTORY_SEPARATOR, $parts) . \".php\"));\n"
        "if (is_file($expected_path) && is_readable($expected_path)) {\n"
    );
    printer->Indent();
    printer->Print(
        "require $expected_path;\n"
        "$retval = true;\n"
    );
    printer->Outdent();
    printer->Print(
        "}\n"
    );
    printer->Print(
        "return $retval;\n"
    );
    printer->Outdent();
    printer->Print(
        "}\n"
    );
    printer->Outdent();
    printer->Print(
        "}\n"
    );
    printer->Outdent();
    printer->Print(
        "}\n"
    );
    printer->Outdent();
    printer->Print(
        "}\n"
    );
    printer->Print("Autoloader::register();\n");
}


void FileGenerator::GenerateSiblings(const string& package_dir,
                                     GeneratorContext* context,
                                     vector<string>* file_list)
{
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
// TODO:
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
