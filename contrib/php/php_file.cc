#include "php_generator.h"
#include "php_file.h"
#include "php_enum.h"
#include "php_message.h"

#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace php {

FileGenerator::FileGenerator(const FileDescriptor* file)
    : file_(file)
{

}


FileGenerator::~FileGenerator()
{
}

bool FileGenerator::Validate(string* error) {
    return true;
}

void FileGenerator::Generate(io::Printer* printer)
{
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

//  if (!java_package.empty()) {
//    printer.Print(
//      "package $package$;\n"
//      "\n",
//      "package", java_package);
//  }

  GeneratorClass generator(descriptor, context);
  (generator.*pfn)(&printer);

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
