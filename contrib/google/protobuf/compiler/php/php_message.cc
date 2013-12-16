// Copyright 2013 Shuhei Tanuma.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <google/protobuf/compiler/php/php_generator.h>
#include <google/protobuf/compiler/php/php_message.h>
#include <google/protobuf/compiler/php/php_enum.h>
#include <google/protobuf/compiler/php/php_helpers.h>

#include <google/protobuf/descriptor.h>

#include <map>
#include <string>
#include <algorithm>

namespace google {
namespace protobuf {
namespace compiler {
namespace php {

MessageGenerator::MessageGenerator(const Descriptor* descriptor,
    GeneratorContext* context)
    : descriptor_(descriptor),
      context_(context) {
  use_namespace_ = true;
  enclose_namespace_ = false;
}

MessageGenerator::~MessageGenerator() {
}

string MessageGenerator::NameSpace() {
  vector<string> result;
  string output;

  SplitStringUsing(descriptor_->full_name(), ".", &result);
  result.pop_back();

  JoinStrings(result,
    "\\",
    &output);

  return output;
}

bool MessageGenerator::HasNameSpace() {
  string name = NameSpace();

  if (!name.empty()) {
    return true;
  } else {
    return false;
  }
}


string MessageGenerator::ClassName() {
  vector<string> result;

  SplitStringUsing(descriptor_->full_name(), ".", &result);

  return result.back();
}

string MessageGenerator::ClassName(const Descriptor &descriptor) const {
  string result = "\\";
  result.append(descriptor.full_name());

  replace(result.begin(), result.end(), '.', '\\');

  return result;
}

string MessageGenerator::ClassName(const EnumDescriptor &descriptor) const {
  string result = "\\";
  result.append(descriptor.full_name());

  replace(result.begin(), result.end(), '.', '\\');

  return result;
}


string MessageGenerator::FileName() {
  string name = descriptor_->full_name();
  replace(name.begin(), name.end(), '.', '/');
  name.append(".php");

  return name;
}


static const char *fields_map[] = {
  "DUMMY",
  "ProtocolBuffers::TYPE_DOUBLE",
  "ProtocolBuffers::TYPE_FLOAT",
  "ProtocolBuffers::TYPE_INT64",
  "ProtocolBuffers::TYPE_UINT64",
  "ProtocolBuffers::TYPE_INT32",
  "ProtocolBuffers::TYPE_FIXED64",
  "ProtocolBuffers::TYPE_FIXED32",
  "ProtocolBuffers::TYPE_BOOL",
  "ProtocolBuffers::TYPE_STRING",
  "ProtocolBuffers::TYPE_GROUP",
  "ProtocolBuffers::TYPE_MESSAGE",
  "ProtocolBuffers::TYPE_BYTES",
  "ProtocolBuffers::TYPE_UINT32",
  "ProtocolBuffers::TYPE_ENUM",
  "ProtocolBuffers::TYPE_SFIXED32",
  "ProtocolBuffers::TYPE_SFIXED64",
  "ProtocolBuffers::TYPE_SINT32",
  "ProtocolBuffers::TYPE_SINT64",
};

static const char* field_type_to_str(int field_type) {
  if (field_type > 0 && field_type <= 18) {
    return fields_map[field_type];
  } else {
    return NULL;
  }
}

string MessageGenerator::getTypeName(const FieldDescriptor & field) const {
  if (field.is_repeated()) {
    return "array";
  }

  // NOTE: Currently, We use same type for PHP's platform depends reason.
  switch (field.type()) {
    case FieldDescriptorProto_Type_TYPE_DOUBLE:
      return "string";
    break;
    case FieldDescriptorProto_Type_TYPE_FLOAT:
      return "string";
    break;
    case FieldDescriptorProto_Type_TYPE_INT64:
      return "string";
    break;
    case FieldDescriptorProto_Type_TYPE_UINT64:
      return "string";
    break;
    case FieldDescriptorProto_Type_TYPE_INT32:
      return "string";
    break;
    case FieldDescriptorProto_Type_TYPE_FIXED64:
      return "string";
    break;
    case FieldDescriptorProto_Type_TYPE_FIXED32:
      return "string";
    break;
    case FieldDescriptorProto_Type_TYPE_BOOL:
      return "bool";
    break;
    case FieldDescriptorProto_Type_TYPE_STRING:
      return "string";
    break;
    case FieldDescriptorProto_Type_TYPE_GROUP:
      return "";
    break;
    case FieldDescriptorProto_Type_TYPE_MESSAGE: {
      const Descriptor &desc(*field.message_type());
      string name = ClassName(desc);
      return name;
    }
    break;
    case FieldDescriptorProto_Type_TYPE_BYTES:
      return "string";
    break;
    case FieldDescriptorProto_Type_TYPE_UINT32:
      return "string";
    break;
    case FieldDescriptorProto_Type_TYPE_ENUM:
      return "int";
    break;
    case FieldDescriptorProto_Type_TYPE_SFIXED32:
      return "string";
    break;
    case FieldDescriptorProto_Type_TYPE_SFIXED64:
      return "string";
    break;
    case FieldDescriptorProto_Type_TYPE_SINT32:
      return "string";
    break;
    case FieldDescriptorProto_Type_TYPE_SINT64:
      return "string";
    break;
  }
  return "";
}

void MessageGenerator::PrintMagicMethod(io::Printer* printer) {
  printer->Print(" * -*- magic methods -*-\n");
  printer->Print(" *\n");
  for (int i = 0; i < descriptor_->field_count(); ++i) {
    const FieldDescriptor &field(*descriptor_->field(i));
    printer->Print(" * @method `type` get`variable`()\n",
      "type", getTypeName(field),
      "variable", UnderscoresToCapitalizedCamelCase(field));

    if (field.is_repeated()) {
      printer->Print(" * @method void append`variable`()\n",
      "type", getTypeName(field),
      "variable", UnderscoresToCapitalizedCamelCase(field));
    } else {
      printer->Print(" * @method void set`variable`()\n",
      "variable", UnderscoresToCapitalizedCamelCase(field));
    }
  }
  printer->Print(" *\n");
}

void MessageGenerator::PrintUseNameSpaceIfNeeded(io::Printer* printer) {
  if (HasNameSpace()) {
    if (enclose_namespace_) {
      printer->Print(
      "namespace `namespace`\n{\n\n",
      "namespace",
      NameSpace());
    } else {
      printer->Print(
      "namespace `namespace`;\n\n",
      "namespace",
      NameSpace());
    }
  }

  printer->Print("use \\ProtocolBuffers;\n");
  printer->Print("use \\ProtocolBuffers\\Message;\n");
  printer->Print("use \\ProtocolBuffers\\FieldDescriptor;\n");
  printer->Print("use \\ProtocolBuffers\\DescriptorBuilder;\n");
  printer->Print("use \\ProtocolBuffers\\ExtensionRegistry;\n");

  // TODO(chobie): add Message and Enum class here.

  printer->Print("\n");
}

string MessageGenerator::GetClassNameForFieldDescriptor() {
  if (use_namespace_) {
    return "FieldDescriptor";
  } else {
    return "ProtocolBuffersFieldDescriptor";
  }
}

string MessageGenerator::GetClassNameForDescriptorBuilder() {
  if (use_namespace_) {
    return "DescriptorBuilder";
  } else {
    return "ProtocolBuffersDescriptorBuilder";
  }
}

string MessageGenerator::VariableName(const FieldDescriptor & field) const {
  return field.name();
}

string MessageGenerator::DefaultValueAsString(const FieldDescriptor & field,
  bool quote_string_type) const {
  switch (field.cpp_type()) {
    case FieldDescriptor::CPPTYPE_INT32:
      return SimpleItoa(field.default_value_int32());
    case FieldDescriptor::CPPTYPE_INT64:
      return SimpleItoa(field.default_value_int64());
    case FieldDescriptor::CPPTYPE_UINT32:
      return SimpleItoa(field.default_value_uint32());
    case FieldDescriptor::CPPTYPE_UINT64:
      return SimpleItoa(field.default_value_uint64());
    case FieldDescriptor::CPPTYPE_FLOAT:
      return SimpleFtoa(field.default_value_float());
    case FieldDescriptor::CPPTYPE_DOUBLE:
      return SimpleDtoa(field.default_value_double());
    case FieldDescriptor::CPPTYPE_BOOL:
      return (field.default_value_bool()) ? "true" : "false";
    case FieldDescriptor::CPPTYPE_STRING:
      if (quote_string_type)
        return "\"" + CEscape(field.default_value_string()) + "\"";
      if (field.type() == FieldDescriptor::TYPE_BYTES)
        return CEscape(field.default_value_string());

      return field.default_value_string();
    case FieldDescriptor::CPPTYPE_ENUM: {
      const EnumDescriptor &desc(*field.enum_type());
      string value = ClassName(desc);

      value = value + "::" + field.default_value_enum()->name();
      return value;
    }
    case FieldDescriptor::CPPTYPE_MESSAGE:
      return "null";
  }
  return "";
}


void MessageGenerator::PrintGetDescriptor(io::Printer* printer) {
  const PHPMessageOptions& moptions =
    descriptor_->options().GetExtension(::php_option);
  printer->Print("/**\n");
  printer->Print(" * get descriptor for protocol buffers\n");
  printer->Print(" * \n");
  printer->Print(" * @return ProtocolBuffersDescriptor\n");
  printer->Print(" */\n");
  printer->Print("public static function getDescriptor()\n");
  printer->Print("{\n");
  printer->Indent();
  printer->Print("static $descriptor;\n");
  printer->Print("\n");
  printer->Print("if (!isset($descriptor)) {\n");
  printer->Indent();
  printer->Print("$desc = new `class_name`();\n",
    "class_name",
    GetClassNameForDescriptorBuilder());

  for (int i = 0; i < descriptor_->field_count(); ++i) {
    const FieldDescriptor &field(*descriptor_->field(i));

    printer->Print("$desc->addField(`tag`, new `class_name`(array(\n",
      "tag",
      SimpleItoa(field.number()),
      "class_name",
      GetClassNameForFieldDescriptor());
    printer->Indent();
    printer->Print("\"type\"     => `type`,\n",
      "type",
      field_type_to_str(field.type()));
    printer->Print("\"name\"     => \"`name`\",\n",
      "name",
      VariableName(field));
    printer->Print("\"required\" => `required`,\n",
      "required",
      (field.is_required()) ? "true" : "false");
    printer->Print("\"optional\" => `optional`,\n",
      "optional",
      (field.is_optional()) ? "true" : "false");
    printer->Print("\"repeated\" => `repeated`,\n",
      "repeated",
      (field.is_repeated()) ? "true" : "false");
    printer->Print("\"packable\" => `packable`,\n",
     "packable",
      (field.is_packed()) ? "true" : "false");
    printer->Print("\"default\"  => `value`,\n",
      "value",
      DefaultValueAsString(field, true));

    if (field.type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
      const Descriptor &desc(*field.message_type());

      string name = ClassName(desc);
      string escapedName;
      size_t pos = 0;
      size_t found = 0;

      escapedName.reserve(name.length()*2);

      while ((found = name.find('\\', pos)) != string::npos) {
        if (found-pos > 0) {
          escapedName.append(name.substr(pos, found-pos));
        }
        escapedName.append("\\\\");
        pos = found+1;
      }

      escapedName.append(name.substr(pos, found-pos));
      printer->Print("\"message\"  => \"`message`\",\n",
      "message",
      escapedName);
    }

    printer->Outdent();
    printer->Print(")));\n");
  }

  if (moptions.use_single_property()) {
    printer->Print("$phpoptions = $desc->getOptions()->getExtension");
    printer->Print("(ProtocolBuffers::PHP_MESSAGE_OPTION);\n");
    printer->Print("$phpoptions->setUseSingleProperty(true);\n");
    printer->Print("$phpoptions->setSinglePropertyName(\"`name`\");\n",
      "name",
      moptions.single_property_name());

    printer->Print("\n");
  }

  for (int i = 0; i < descriptor_->extension_range_count(); ++i) {
    const Descriptor::ExtensionRange &er(*descriptor_->extension_range(i));
    printer->Print("$desc->addExtensionRange(`start`, `end`);\n",
      "start",
      SimpleItoa(er.start),
      "end", SimpleItoa(er.end));
  }


  printer->Print("$descriptor = $desc->build();\n");
  printer->Outdent();
  printer->Print("}\n");

  printer->Print("\n");
  printer->Print("return $descriptor;\n");
  printer->Outdent();
  printer->Print("}\n");
  printer->Print("\n");
}

void MessageGenerator::PrintTraits(io::Printer* printer) {
  if (descriptor_->options().HasExtension(php_option)) {
    vector<string> traits;
    vector<string>::iterator it;

    SplitStringUsing(descriptor_->options().GetExtension(php_option).trait(), " ", &traits);

    it = traits.begin();
    while (it != traits.end()) {

      printer->Print("use `trait`;\n", "trait", *it);
      ++it;
    }

    printer->Print("\n");
  }
}

void MessageGenerator::PrintMemberProperties(io::Printer* printer) {
  const PHPMessageOptions& moptions =
    descriptor_->options().GetExtension(::php_option);

  if (moptions.use_single_property()) {
    printer->Print("/** @var array $`var` */\n", "var",
      moptions.single_property_name());
    printer->Print("protected $`var` = array();\n",
      "var",
      moptions.single_property_name());
  } else {
    for (int i = 0; i < descriptor_->field_count(); ++i) {
      const FieldDescriptor &field(*descriptor_->field(i));

      if (field.is_repeated()) {
        printer->Print("/** @var `type` $`var` tag:`tag` ",
          "type", getTypeName(field),
          "var", field.name(),
          "tag", SimpleItoa(field.number()));
        printer->Print(" `required` `field` */\n",
          "required", (field.is_required() ? "required" : "optional"),
          "field", (string)field_type_to_str(field.type()));

        printer->Print("protected $`var` = array();\n",
          "var", field.name());
      } else {
        printer->Print("/** @var `type` $`var` tag:`tag` ",
          "type", getTypeName(field),
          "var", field.name(),
          "tag", SimpleItoa(field.number()));
        printer->Print(" `required` `field` */\n",
          "required", (field.is_required() ? "required" : "optional"),
          "field", (string)field_type_to_str(field.type()));

        printer->Print("protected $`var`;\n",
          "var", field.name());
      }

      printer->Print("\n");
    }
  }
}

void MessageGenerator::Generate(io::Printer* printer) {
  for (int i = 0; i < descriptor_->enum_type_count(); i++) {
    EnumGenerator enumGenerator(descriptor_->enum_type(i), context_);
    string child_name = enumGenerator.FileName();

    scoped_ptr<io::ZeroCopyOutputStream> output(context_->Open(child_name));
    io::Printer child_printer(output.get(), '`');
    enumGenerator.Generate(&child_printer);
  }

  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    MessageGenerator messageGenerator(descriptor_->nested_type(i), context_);
    string child_name = messageGenerator.FileName();

    scoped_ptr<io::ZeroCopyOutputStream> output(context_->Open(child_name));
    io::Printer child_printer(output.get(), '`');
    messageGenerator.Generate(&child_printer);
  }

  printer->Print("<?php\n");
  PrintUseNameSpaceIfNeeded(printer);

  if (enclose_namespace_) {
    printer->Indent();
  }

  printer->Print(
    "/**\n"
    " * Generated by the protocol buffer compiler.  DO NOT EDIT!\n"
    " * source: `filename`\n"
    " *\n"
    ,
    "filename", descriptor_->file()->name());

  PrintMagicMethod(printer);

  printer->Print(" */\n");

  printer->Print(
    "class `class_name` extends Message\n"
    "{\n"
    ,
    "class_name", ClassName());

  printer->Indent();
  PrintTraits(printer);
  PrintMemberProperties(printer);

  PrintGetDescriptor(printer);

  printer->Outdent();
  printer->Print("}\n\n");

  if (enclose_namespace_) {
    printer->Outdent();
    printer->Print("}\n\n");
  }
}

}  // namespace php
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
