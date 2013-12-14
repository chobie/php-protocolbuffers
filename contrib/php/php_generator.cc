#include "php_generator.h"
#include "strutil.h" // TODO This header is from the offical protobuf source, but it is not normally installed

namespace google {
namespace protobuf {
namespace compiler {
namespace php {

PHPGenerator::PHPGenerator(){
}

PHPGenerator::~PHPGenerator(){
}

bool PHPGenerator::Generate(const FileDescriptor* file,
                         const string& parameter,
                         GeneratorContext* context,
                         string* error) const {
    bool result = true;
    string output_list_file;
    vector<string> all_files;
    vector<pair<string, string> > options;
    ParseGeneratorParameter(parameter, &options);

    for (int i = 0; i < options.size(); i++) {
        if (options[i].first == "output_list_file") {
            output_list_file = options[i].second;
        } else {
            *error = "Unknown generator option: " + options[i].first;
            return false;
        }
    }

    FileGenerator file_generator(file, context);
    if (!file_generator.Validate(error)) {
        return false;
    }

// TODO: Generate php main file.
    scoped_ptr<io::ZeroCopyOutputStream> output(
        context->Open("autoload.php"));
    io::Printer printer(output.get(), '`');
    file_generator.Generate(&printer);

    file_generator.GenerateSiblings("", context, &all_files);
    if (!output_list_file.empty()) {
        // TODO: Outputs file
    }

    return result;
}

}  // namespace php
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
