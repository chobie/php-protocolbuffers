/*
 * Protocol Buffers for PHP
 * Copyright 2013 Shuhei Tanuma.  All rights reserved.
 *
 * https://github.com/chobie/php-protocolbuffers
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "protocolbuffers.h"

#include "descriptor.h"
#include "field_descriptor.h"
#include "message.h"
#include "message_options.h"
#include "enum.h"
#include "php_message_options.h"
#include "descriptor_builder.h"
#include "unknown_field.h"
#include "unknown_field_set.h"
#include "extension_registry.h"
#include "helper.h"
#include "enum_descriptor.h"
#include "enum_value_descriptor.h"
#include "enum_descriptor_builder.h"


zend_class_entry *protocol_buffers_class_entry;
zend_class_entry *php_protocol_buffers_descriptor_class_entry;
zend_class_entry *php_protocol_buffers_field_descriptor_class_entry;

zend_class_entry *php_protocol_buffers_serializable_class_entry;
zend_class_entry *php_protocol_buffers_enum_describable_class_entry;

zend_class_entry *php_protocol_buffers_message_class_entry;
zend_class_entry *php_protocol_buffers_message_options_class_entry;
zend_class_entry *php_protocol_buffers_enum_class_entry;
zend_class_entry *php_protocol_buffers_enum_descriptor_class_entry;
zend_class_entry *php_protocol_buffers_enum_value_descriptor_class_entry;
zend_class_entry *php_protocol_buffers_enum_descriptor_builder_class_entry;
zend_class_entry *php_protocol_buffers_descriptor_builder_class_entry;
zend_class_entry *php_protocol_buffers_unknown_field_class_entry;
zend_class_entry *php_protocol_buffers_unknown_field_set_class_entry;
zend_class_entry *php_protocol_buffers_helper_class_entry;
zend_class_entry *php_protocol_buffers_extension_registry_class_entry;

zend_class_entry *php_protocol_buffers_php_message_options_class_entry;

zend_class_entry *php_protocol_buffers_invalid_byte_sequence_class_entry;
zend_class_entry *php_protocol_buffers_invalid_protocolbuffers_exception_class_entry;
zend_class_entry *php_protocol_buffers_uninitialized_message_exception_class_entry;

ZEND_DECLARE_MODULE_GLOBALS(protocolbuffers);

zend_class_entry *php_protocolbuffers_get_exception_base(TSRMLS_D)
{
#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 2)
	return zend_exception_get_default();
#else
	return zend_exception_get_default(TSRMLS_C);
#endif
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, class_name)
	ZEND_ARG_INFO(0, bytes)
	ZEND_ARG_INFO(0, descriptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_encode, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, descriptor)
ZEND_END_ARG_INFO()

/* {{{ proto mixed ProtocolBuffers::decode($class_name, $bytes)
*/
PHP_METHOD(protocolbuffers, decode)
{
	char *klass;
	const char *data;
	long klass_len = 0, data_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"ss", &klass, &klass_len, &data, &data_len) == FAILURE) {
		return;
	}

	if (klass[0] == '\\') {
		int x;
		for (x = 0; x < klass_len-1; x++) {
			klass[x] = klass[x+1];
		}
		klass[klass_len-1] = '\0';
		klass_len--;
	}

	php_protocolbuffers_decode(INTERNAL_FUNCTION_PARAM_PASSTHRU, data, data_len, klass, klass_len);
}
/* }}} */

/* {{{ proto string ProtocolBuffers::encode($class)
*/
PHP_METHOD(protocolbuffers, encode)
{
	zval *klass = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"o", &klass) == FAILURE) {
		return;
	}

	php_protocolbuffers_encode(INTERNAL_FUNCTION_PARAM_PASSTHRU, Z_OBJCE_P(klass), klass);
}
/* }}} */

static zend_function_entry php_protocolbuffers_methods[] = {
	PHP_ME(protocolbuffers, decode, arginfo_protocolbuffers_decode, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers, encode, NULL, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_FE_END
};

static void php_protocol_buffers_invalid_byte_sequence_exception(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersInvalidByteSequenceException", 0);
	php_protocol_buffers_invalid_byte_sequence_class_entry = zend_register_internal_class_ex(&ce, php_protocolbuffers_get_exception_base(TSRMLS_C), NULL TSRMLS_CC);

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "InvalidByteSequenceException", php_protocol_buffers_invalid_byte_sequence_class_entry);
}

static void php_protocol_buffers_invalid_exception(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersInvalidProtocolBufferException", 0);
	php_protocol_buffers_invalid_protocolbuffers_exception_class_entry = zend_register_internal_class_ex(&ce, php_protocolbuffers_get_exception_base(TSRMLS_C), NULL TSRMLS_CC);

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "InvalidProtocolBufferException", php_protocol_buffers_invalid_protocolbuffers_exception_class_entry);
}

static void php_protocolbuffers_uninitialized_message_exception(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersUninitializedMessageException", 0);
	php_protocol_buffers_uninitialized_message_exception_class_entry = zend_register_internal_class_ex(&ce, php_protocolbuffers_get_exception_base(TSRMLS_C), NULL TSRMLS_CC);

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "UninitializedMessageException", php_protocol_buffers_uninitialized_message_exception_class_entry);
}

static zend_function_entry php_protocolbuffers_describable_methods[] = {
	ZEND_FENTRY(getDescriptor, NULL, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC | ZEND_ACC_ABSTRACT)
	PHP_FE_END
};

static void php_protocol_buffers_serializable(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersDescribable", php_protocolbuffers_describable_methods);
	php_protocol_buffers_serializable_class_entry = zend_register_internal_interface(&ce TSRMLS_CC);

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "Describable", php_protocol_buffers_serializable_class_entry);
}

static zend_function_entry php_protocolbuffers_enum_describable_methods[] = {
	ZEND_FENTRY(getEnumDescriptor, NULL, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC | ZEND_ACC_ABSTRACT)
	PHP_FE_END
};

static void php_protocol_buffers_enum_describable(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersEnumDescribable", php_protocolbuffers_enum_describable_methods);
	php_protocol_buffers_enum_describable_class_entry = zend_register_internal_interface(&ce TSRMLS_CC);

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "EnumDescribable", php_protocol_buffers_serializable_class_entry);
}

static void php_protocolbuffers_init(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffers", php_protocolbuffers_methods);
	protocol_buffers_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	php_protocol_buffers_invalid_byte_sequence_exception(TSRMLS_C);
	php_protocol_buffers_invalid_exception(TSRMLS_C);
	php_protocolbuffers_uninitialized_message_exception(TSRMLS_C);
	php_protocolbuffers_descriptor_class(TSRMLS_C);
	php_protocolbuffers_filed_descriptor_class(TSRMLS_C);
	php_protocol_buffers_serializable(TSRMLS_C);
	php_protocol_buffers_enum_describable(TSRMLS_C);
	php_protocolbuffers_message_class(TSRMLS_C);
	php_protocolbuffers_message_options_class(TSRMLS_C);
	php_protocolbuffers_enum_class(TSRMLS_C);
	php_protocolbuffers_enum_descriptor_class(TSRMLS_C);
	php_protocolbuffers_enum_value_descriptor_class(TSRMLS_C);
	php_protocolbuffers_enum_descriptor_builder_class(TSRMLS_C);
	php_protocolbuffers_php_message_options_class(TSRMLS_C);
	php_protocolbuffers_descriptor_builder_class(TSRMLS_C);
	php_protocolbuffers_unknown_field_class(TSRMLS_C);
	php_protocolbuffers_unknown_field_set_class(TSRMLS_C);
	php_protocolbuffers_helper_class(TSRMLS_C);
	php_protocolbuffers_extension_registry_class(TSRMLS_C);

#define PB_DECLARE_CONST_STRING(name, value) \
	zend_declare_class_constant_string(protocol_buffers_class_entry, ZEND_STRS(#name)-1, "php" TSRMLS_CC);

#define PB_DECLARE_CONST_LONG(name, value) \
	zend_declare_class_constant_long(protocol_buffers_class_entry, ZEND_STRS(#name)-1, value TSRMLS_CC);

	PB_DECLARE_CONST_LONG(WIRETYPE_VARINT,           WIRETYPE_VARINT);
	PB_DECLARE_CONST_LONG(WIRETYPE_FIXED64,          WIRETYPE_FIXED64);
	PB_DECLARE_CONST_LONG(WIRETYPE_LENGTH_DELIMITED, WIRETYPE_LENGTH_DELIMITED);
	PB_DECLARE_CONST_LONG(WIRETYPE_START_GROUP,      WIRETYPE_START_GROUP);
	PB_DECLARE_CONST_LONG(WIRETYPE_END_GROUP,        WIRETYPE_END_GROUP);
	PB_DECLARE_CONST_LONG(WIRETYPE_FIXED32,          WIRETYPE_FIXED32);

	PB_DECLARE_CONST_LONG(TYPE_DOUBLE,    TYPE_DOUBLE);
	PB_DECLARE_CONST_LONG(TYPE_FLOAT,     TYPE_FLOAT);
	PB_DECLARE_CONST_LONG(TYPE_INT64,     TYPE_INT64);
	PB_DECLARE_CONST_LONG(TYPE_UINT64,    TYPE_UINT64);
	PB_DECLARE_CONST_LONG(TYPE_INT32,     TYPE_INT32);
	PB_DECLARE_CONST_LONG(TYPE_FIXED64,   TYPE_FIXED64);
	PB_DECLARE_CONST_LONG(TYPE_FIXED32,   TYPE_FIXED32);
	PB_DECLARE_CONST_LONG(TYPE_BOOL,      TYPE_BOOL);
	PB_DECLARE_CONST_LONG(TYPE_STRING,    TYPE_STRING);
	PB_DECLARE_CONST_LONG(TYPE_GROUP,     TYPE_GROUP);
	PB_DECLARE_CONST_LONG(TYPE_MESSAGE,   TYPE_MESSAGE);
	PB_DECLARE_CONST_LONG(TYPE_BYTES,     TYPE_BYTES);
	PB_DECLARE_CONST_LONG(TYPE_UINT32,    TYPE_UINT32);
	PB_DECLARE_CONST_LONG(TYPE_ENUM,      TYPE_ENUM);
	PB_DECLARE_CONST_LONG(TYPE_SFIXED32,  TYPE_SFIXED32);
	PB_DECLARE_CONST_LONG(TYPE_SFIXED64,  TYPE_SFIXED64);
	PB_DECLARE_CONST_LONG(TYPE_SINT32,    TYPE_SINT32);
	PB_DECLARE_CONST_LONG(TYPE_SINT64,    TYPE_SINT64);
	PB_DECLARE_CONST_LONG(MAX_FIELD_TYPE, MAX_FIELD_TYPE);

	PB_DECLARE_CONST_STRING(PHP_MESSAGE_OPTION, "php");

#undef PB_DECLARE_CONST_LONG

}

PHP_MINFO_FUNCTION(protocolbuffers)
{
	php_printf("PHP Protocol Buffers Extension\n");

	php_info_print_table_start();
	php_info_print_table_header(2, "Protocol Buffers Support", "enabled");
	php_info_print_table_row(2, "Version", PHP_PROTOCOLBUFFERS_VERSION);
	php_info_print_table_end();
}

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("protocolbuffers.strict_mode", "1", PHP_INI_ALL, OnUpdateLong, strict_mode, zend_protocolbuffers_globals, protocolbuffers_globals)
	STD_PHP_INI_BOOLEAN("protocolbuffers.validate_string", "1", PHP_INI_ALL, OnUpdateLong, validate_string, zend_protocolbuffers_globals, protocolbuffers_globals)
	STD_PHP_INI_BOOLEAN("protocolbuffers.native_scalar", "0", PHP_INI_ALL, OnUpdateLong, native_scalar, zend_protocolbuffers_globals, protocolbuffers_globals)
PHP_INI_END()

static PHP_GINIT_FUNCTION(protocolbuffers)
{
}

static PHP_GSHUTDOWN_FUNCTION(protocolbuffers)
{
}

PHP_MINIT_FUNCTION(protocolbuffers)
{
	REGISTER_INI_ENTRIES();

	php_protocolbuffers_init(TSRMLS_C);
	return SUCCESS;
}

PHP_RINIT_FUNCTION(protocolbuffers)
{
	zend_class_entry **json;
	PBG(messages) = NULL;
	PBG(classes) = NULL;
	PBG(extension_registry) = NULL;
	PBG(strict_mode) = 1;

	// NOTE(chobie): prevent segmentaiton fault on CentOS box (CentOS uses json shared modules.)
	if (zend_lookup_class("JsonSerializable", sizeof("JsonSerializable")-1, &json TSRMLS_CC) != FAILURE) {
		zend_class_implements(php_protocol_buffers_message_class_entry TSRMLS_CC, 1, *json);
	}

	if (!PBG(messages)) {
		ALLOC_HASHTABLE(PBG(messages));
		zend_hash_init(PBG(messages), 0, NULL, NULL, 0);
	}

	if (!PBG(classes)) {
		ALLOC_HASHTABLE(PBG(classes));
		zend_hash_init(PBG(classes), 0, NULL, NULL, 0);
	}
	PBG(validate_string) = 1;
	PBG(native_scalar) = 0;

	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(protocolbuffers)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(protocolbuffers)
{
		if (PBG(messages)) {
			zend_try {
			int i = 0;
			HashPosition pos;
			php_protocolbuffers_scheme_container **element;

			for(zend_hash_internal_pointer_reset_ex(PBG(messages), &pos);
							zend_hash_get_current_data_ex(PBG(messages), (void **)&element, &pos) == SUCCESS;
							zend_hash_move_forward_ex(PBG(messages), &pos)
			) {
				for (i = 0; i < (*element)->size; i++) {
					if ((*element)->scheme[i].original_name != NULL) {
						efree((*element)->scheme[i].original_name);
					}
					if ((*element)->scheme[i].name != NULL) {
						efree((*element)->scheme[i].name);
					}
					if ((*element)->scheme[i].mangled_name != NULL) {
						efree((*element)->scheme[i].mangled_name);
					}
					if ((*element)->scheme[i].default_value != NULL) {
						zval_ptr_dtor(&((*element)->scheme[i].default_value));
					}
				}

				if ((*element)->single_property_name != NULL) {
					efree((*element)->single_property_name);
				}

				if ((*element)->orig_single_property_name != NULL &&
					memcmp((*element)->orig_single_property_name, php_protocolbuffers_get_default_single_property_name(), php_protocolbuffers_get_default_single_property_name_len()) != 0) {
					efree((*element)->orig_single_property_name);
				}

				if ((*element)->scheme != NULL) {
					efree((*element)->scheme);
				}

				if ((*element)->extensions != NULL) {
					efree((*element)->extensions);
				}

				efree(*element);
			}

			zend_hash_destroy(PBG(messages));
			FREE_HASHTABLE(PBG(messages));
			PBG(messages) = NULL;
		}
		zend_end_try();
	}

	if (PBG(classes)) {
		zend_try {
			zend_hash_destroy(PBG(classes));
			FREE_HASHTABLE(PBG(classes));
			PBG(classes) = NULL;
		}
		zend_end_try();
	}

	if (PBG(extension_registry)) {
		zval_ptr_dtor(&PBG(extension_registry));
	}


	return SUCCESS;
}


zend_module_entry protocolbuffers_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"protocolbuffers",
	NULL,					/* Functions */
	PHP_MINIT(protocolbuffers),	/* MINIT */
	PHP_MSHUTDOWN(protocolbuffers),	/* MSHUTDOWN */
	PHP_RINIT(protocolbuffers),	/* RINIT */
	PHP_RSHUTDOWN(protocolbuffers),		/* RSHUTDOWN */
	PHP_MINFO(protocolbuffers),	/* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
	PHP_PROTOCOLBUFFERS_VERSION,
#endif
	PHP_MODULE_GLOBALS(protocolbuffers),
	PHP_GINIT(protocolbuffers),
	PHP_GSHUTDOWN(protocolbuffers),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};


#ifdef COMPILE_DL_PROTOCOLBUFFERS
ZEND_GET_MODULE(protocolbuffers)
#endif
