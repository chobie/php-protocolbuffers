#ifndef PHP_PROTOC_GEN_INCLUDED
#define PHP_PROTOC_GEN_INCLUDED

#include "strutil.h" // TODO This header is from the offical protobuf source, but it is not normally installed

#include <map>
#include <string>
#include <algorithm>

#include <cstdio> // for sprintf

#include <google/protobuf/descriptor.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/wire_format_lite_inl.h>


#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/code_generator.h>

#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include "php_options.pb.h"

using namespace google::protobuf;
using namespace google::protobuf::compiler;
using namespace google::protobuf::internal;

class PHPCodeGenerator : public CodeGenerator {
	private:

		void PrintMessage   (io::Printer &printer, const Descriptor & message) const;
		void PrintMessages  (io::Printer &printer, const FileDescriptor & file) const;

		void PrintEnum      (io::Printer &printer, const EnumDescriptor & e) const;
		void PrintEnums     (io::Printer &printer, const FileDescriptor & file) const;

		void PrintService   (io::Printer &printer, const ServiceDescriptor & service) const;
		void PrintServices  (io::Printer &printer, const FileDescriptor & file) const;

		string DefaultValueAsString(const FieldDescriptor & field, bool quote_string_type) const;

		// Print the read() method
		void PrintMessageRead(io::Printer &printer, const Descriptor & message, vector<const FieldDescriptor *> & required_fields, const FieldDescriptor * parentField) const;

		// Print the write() method
		void PrintMessageWrite(io::Printer &printer, const Descriptor & message, const FieldDescriptor * parentField) const;

		// Print the size() method
		void PrintMessageSize(io::Printer &printer, const Descriptor & message) const;

		// Maps names into PHP names
		template <class DescriptorType>
		string ClassName(const DescriptorType & descriptor) const;

		string VariableName(const FieldDescriptor & field) const;

	public:

		PHPCodeGenerator();
		~PHPCodeGenerator();

		bool Generate(const FileDescriptor* file, const string& parameter, OutputDirectory* output_directory, string* error) const;

};
#endif