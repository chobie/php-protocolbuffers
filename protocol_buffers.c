/*
 * Protocol Buffers for PHP - Google's data interchange format
 * Copyright 2013 Shuhei Tanuma.  All rights reserved.
 *
 * https://github.com/chobie/php-protocolbuffers
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *   * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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
#include "php_protocol_buffers.h"
#include "protocol_buffers.h"

#include "descriptor.h"
#include "field_descriptor.h"
#include "field_options.h"
#include "message.h"
#include "message_options.h"
#include "php_message_options.h"
#include "descriptor_builder.h"
#include "unknown_field.h"
#include "unknown_field_set.h"

#ifdef ZTS
int pb_globals_id;
#else
pb_globals php_pb_globals;
#endif

zend_class_entry *protocol_buffers_class_entry;
zend_class_entry *protocol_buffers_descriptor_class_entry;
zend_class_entry *protocol_buffers_field_descriptor_class_entry;
zend_class_entry *protocol_buffers_field_options_class_entry;

zend_class_entry *protocol_buffers_serializable_class_entry;

zend_class_entry *protocol_buffers_message_class_entry;
zend_class_entry *protocol_buffers_message_options_class_entry;
zend_class_entry *protocol_buffers_descriptor_builder_class_entry;
zend_class_entry *protocol_buffers_unknown_field_class_entry;
zend_class_entry *protocol_buffers_unknown_field_set_class_entry;

zend_class_entry *protocol_buffers_php_message_options_class_entry;

zend_class_entry *protocol_buffers_invalid_byte_sequence_class_entry;
zend_class_entry *protocol_buffers_invalid_protocolbuffers_exception_class_entry;
zend_class_entry *protocol_buffers_uninitialized_message_exception_class_entry;

static zend_class_entry *php_pb_get_exception_base(TSRMLS_D)
{
#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 2)
		return zend_exception_get_default();
#else
		return zend_exception_get_default(TSRMLS_C);
#endif
}

static void pb_globals_ctor(pb_globals *pb_globals_p TSRMLS_DC)
{
}

static void pb_globals_dtor(pb_globals *pb_globals_p TSRMLS_DC)
{
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, class_name)
	ZEND_ARG_INFO(0, bytes)
	ZEND_ARG_INFO(0, descriptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_encode, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, descriptor)
ZEND_END_ARG_INFO()


/* {{{ proto mixed ProtocolBuffers::decode($class_name, $bytes)
*/
PHP_METHOD(protocolbuffers, decode)
{
	HashTable *proto = NULL;
	char *klass;
	const char *data, *data_end, *res;
	long klass_len = 0, data_len = 0;
	long buffer_size = 0;
	zval *obj, *z_proto = NULL;
	pb_scheme_container *container;
	int err = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"ss|z", &klass, &klass_len, &data, &data_len, &z_proto) == FAILURE) {
		return;
	}

	if (data_len < 1) {
		zend_throw_exception_ex(protocol_buffers_invalid_protocolbuffers_exception_class_entry, 0 TSRMLS_CC, "passed variable seems null");
		return;
	}

	if (z_proto && Z_TYPE_P(z_proto) == IS_ARRAY) {
		proto	   = Z_ARRVAL_P(z_proto);
	}

	buffer_size = (long)data + sizeof(data);
	err = pb_get_scheme_container(klass, klass_len, &container, proto TSRMLS_CC);
	if (err) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_get_scheme_container failed. %s does not have getDescriptor method", klass);
		return;
	}

	data_end = data + data_len;

	{
		zend_class_entry **ce = NULL;

		if (PBG(classes)) {
			/* Memo: fast lookup */
			if (zend_hash_find(PBG(classes), klass, klass_len, (void **)&ce) == FAILURE) {
				zend_lookup_class(klass, klass_len, &ce TSRMLS_CC);
				if (ce != NULL) {
					zend_hash_update(PBG(classes), klass, klass_len, (void **)ce, sizeof(ce), NULL);
				} else {
					php_error_docref(NULL TSRMLS_CC, E_ERROR, "class lookup failed. %s does exist", klass);
					return;
				}
			}
		}

		MAKE_STD_ZVAL(obj);
		object_init_ex(obj, *ce);

		/* add unknown fields */
		{
			zval *unknown;
			zval **un;
			char *unknown_name;
			int unknown_name_len;

			MAKE_STD_ZVAL(unknown);

			object_init_ex(unknown, protocol_buffers_unknown_field_set_class_entry);
			zend_mangle_property_name(&unknown_name, &unknown_name_len, (char*)"*", 1, (char*)"_unknown", sizeof("_unknown"), 0);
			if (zend_hash_find(Z_OBJPROP_P(obj), (char*)unknown_name, unknown_name_len, (void **)&un) == FAILURE) {
				zend_hash_update(Z_OBJPROP_P(obj), unknown_name, unknown_name_len, (void **)&unknown, sizeof(unknown), NULL);
			} else {
				if (Z_TYPE_PP(un) == IS_NULL) {
					zend_hash_update(Z_OBJPROP_P(obj), unknown_name, unknown_name_len, (void **)&unknown, sizeof(unknown), NULL);
				} else {
					efree(unknown);
				}
			}
			efree(unknown_name);
		}

		res = pb_decode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, data, data_end, container, &obj);
		if (res == NULL) {
			zval_ptr_dtor(&obj);
			zend_throw_exception_ex(protocol_buffers_invalid_protocolbuffers_exception_class_entry, 0 TSRMLS_CC, "passed variable contains malformed byte sequence. or it contains unsupported tag");
			return;
		}

		/* TODO:
		pb_execute_wakeup(obj TSRMLS_CC);
		*/
	}

	RETURN_ZVAL(obj, 0, 1);
}
/* }}} */

/* {{{ proto string ProtocolBuffers::encode($class[, array $descriptor])
*/
PHP_METHOD(protocolbuffers, encode)
{
	zval *klass, *z_descriptor = NULL;
	zend_class_entry *ce;
	pb_scheme_container *container;
	HashTable *proto = NULL;
	pb_serializer *ser = NULL;
	int err = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"o|a", &klass, &z_descriptor) == FAILURE) {
		return;
	}

	ce = Z_OBJCE_P(klass);

	if (z_descriptor) {
		proto	   = Z_ARRVAL_P(z_descriptor);
	}

	err = pb_get_scheme_container(ce->name, ce->name_length, &container, proto TSRMLS_CC);
	if (err) {
		if (EG(exception)) {
			return;
		} else {
			/* TODO: improve displaying error message */
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_get_scheme_cointainer failed. %s does not have getDescriptor method", ce->name);
			return;
		}
	}

	if (pb_encode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, klass, container, &ser)) {
		return;
	}

	if (ser == NULL) {
		RETURN_EMPTY_STRING();
	}

	if (ser->buffer_size > 0) {
		RETVAL_STRINGL((char*)ser->buffer, ser->buffer_size, 1);

		pb_serializer_destroy(ser);
	} else {
		pb_serializer_destroy(ser);
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

static zend_function_entry php_protocolbuffers_methods[] = {
	PHP_ME(protocolbuffers, decode, arginfo_pb_decode, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers, encode, NULL, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};


static void php_invalid_byte_sequence_exception(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffers_InvalidByteSequenceException", 0);
	protocol_buffers_invalid_byte_sequence_class_entry = zend_register_internal_class_ex(&ce, php_pb_get_exception_base(TSRMLS_C), NULL TSRMLS_CC);
}

static void php_protocol_buffers_invalid_exception(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersInvalidProtocolBufferException", 0);
	protocol_buffers_invalid_protocolbuffers_exception_class_entry = zend_register_internal_class_ex(&ce, php_pb_get_exception_base(TSRMLS_C), NULL TSRMLS_CC);

}

static void php_uninitialized_message_exception(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersUninitializedMessage", 0);
	protocol_buffers_uninitialized_message_exception_class_entry = zend_register_internal_class_ex(&ce, php_pb_get_exception_base(TSRMLS_C), NULL TSRMLS_CC);

}


static zend_function_entry php_protocolbuffers_describable_methods[] = {
	ZEND_FENTRY(getDescriptor, NULL, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC | ZEND_ACC_ABSTRACT)
	{NULL, NULL, NULL}
};

static void php_protocol_buffers_serializable(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersDescribable", php_protocolbuffers_describable_methods);
	protocol_buffers_serializable_class_entry = zend_register_internal_interface(&ce TSRMLS_CC);
}

void php_protocolbuffers_init(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffers", php_protocolbuffers_methods);
	protocol_buffers_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	php_invalid_byte_sequence_exception(TSRMLS_C);
	php_protocol_buffers_invalid_exception(TSRMLS_C);
	php_uninitialized_message_exception(TSRMLS_C);
	php_pb_descriptor_class(TSRMLS_C);
	php_pb_filed_descriptor_class(TSRMLS_C);
	php_pb_filed_options_class(TSRMLS_C);
	php_protocol_buffers_serializable(TSRMLS_C);
	php_pb_message_class(TSRMLS_C);
	php_pb_message_options_class(TSRMLS_C);
	php_pb_php_message_options_class(TSRMLS_C);
	php_pb_descriptor_builder_class(TSRMLS_C);
	php_pb_unknown_field_class(TSRMLS_C);
	php_pb_unknown_field_set_class(TSRMLS_C);

#define PB_DECLARE_CONST_STRING(name, size, value) \
	zend_declare_class_constant_string(protocol_buffers_class_entry, name, size, "php" TSRMLS_CC);

#define PB_DECLARE_CONST_LONG(name, size, value) \
	zend_declare_class_constant_long(protocol_buffers_class_entry, name, size, value TSRMLS_CC);

	PB_DECLARE_CONST_LONG("WIRETYPE_VARINT",		   sizeof("WIRETYPE_VARINT")-1,		   0);
	PB_DECLARE_CONST_LONG("WIRETYPE_FIXED64",		  sizeof("WIRETYPE_FIXED64")-1,		  1);
	PB_DECLARE_CONST_LONG("WIRETYPE_LENGTH_DELIMITED", sizeof("WIRETYPE_LENGTH_DELIMITED")-1, 2);
	PB_DECLARE_CONST_LONG("WIRETYPE_START_GROUP",	  sizeof("WIRETYPE_START_GROUP")-1,	  3);
	PB_DECLARE_CONST_LONG("WIRETYPE_END_GROUP",		sizeof("WIRETYPE_END_GROUP")-1,		4);
	PB_DECLARE_CONST_LONG("WIRETYPE_FIXED32",		  sizeof("WIRETYPE_FIXED32")-1,		  5);

	PB_DECLARE_CONST_LONG("TYPE_DOUBLE",			   sizeof("TYPE_DOUBLE")-1,			   1);
	PB_DECLARE_CONST_LONG("TYPE_FLOAT",				sizeof("TYPE_FLOAT")-1,				2);
	PB_DECLARE_CONST_LONG("TYPE_INT64",				sizeof("TYPE_INT64")-1,				3);
	PB_DECLARE_CONST_LONG("TYPE_UINT64",			   sizeof("TYPE_UINT64")-1,			   4);
	PB_DECLARE_CONST_LONG("TYPE_INT32",				sizeof("TYPE_INT32")-1,				5);
	PB_DECLARE_CONST_LONG("TYPE_FIXED64",			  sizeof("TYPE_FIXED64")-1,			  6);
	PB_DECLARE_CONST_LONG("TYPE_FIXED32",			  sizeof("TYPE_FIXED32")-1,			  7);
	PB_DECLARE_CONST_LONG("TYPE_BOOL",				 sizeof("TYPE_BOOL")-1,				 8);
	PB_DECLARE_CONST_LONG("TYPE_STRING",			   sizeof("TYPE_STRING")-1,			   9);
	PB_DECLARE_CONST_LONG("TYPE_GROUP",				sizeof("TYPE_GROUP")-1,			   10);
	PB_DECLARE_CONST_LONG("TYPE_MESSAGE",			  sizeof("TYPE_MESSAGE")-1,			 11);
	PB_DECLARE_CONST_LONG("TYPE_BYTES",				sizeof("TYPE_BYTES")-1,			   12);
	PB_DECLARE_CONST_LONG("TYPE_UINT32",			   sizeof("TYPE_UINT32")-1,			  13);
	PB_DECLARE_CONST_LONG("TYPE_ENUM",				 sizeof("TYPE_ENUM")-1,				14);
	PB_DECLARE_CONST_LONG("TYPE_SFIXED32",			 sizeof("TYPE_SFIXED32")-1,			15);
	PB_DECLARE_CONST_LONG("TYPE_SFIXED64",			 sizeof("TYPE_SFIXED64")-1,			16);
	PB_DECLARE_CONST_LONG("TYPE_SINT32",			   sizeof("TYPE_SINT32")-1,			  17);
	PB_DECLARE_CONST_LONG("TYPE_SINT64",			   sizeof("TYPE_SINT64")-1,			  18);
	PB_DECLARE_CONST_LONG("MAX_FIELD_TYPE",			sizeof("MAX_FIELD_TYPE")-1,		   19);

	PB_DECLARE_CONST_STRING("PHP_MESSAGE_OPTION",		   sizeof("PHP_MESSAGE_OPTION")-1,		   "php");

#undef PB_DECLARE_CONST_LONG

}

PHP_MINFO_FUNCTION(protocolbuffers)
{

	php_printf("PHP Protocol Buffers Extension\n");

	php_info_print_table_start();
	php_info_print_table_header(2, "Protocol Buffers Support", "enabled");
	php_info_print_table_row(2, "Version", PHP_PROTOCOLBUFFERS_EXTVER);
	php_info_print_table_end();
}

PHP_MINIT_FUNCTION(protocolbuffers)
{

#ifdef ZTS
	ts_allocate_id(&pb_globals_id, sizeof(pb_globals), (ts_allocate_ctor) pb_globals_ctor, (ts_allocate_dtor) pb_globals_dtor);
#else
	pb_globals_ctor(&php_pb_globals TSRMLS_CC);
#endif

	php_protocolbuffers_init(TSRMLS_C);
	return SUCCESS;
}

PHP_RINIT_FUNCTION(protocolbuffers)
{
	PBG(messages) = NULL;
	PBG(classes) = NULL;

	if (!PBG(messages)) {
		ALLOC_HASHTABLE(PBG(messages));
		zend_hash_init(PBG(messages), 0, NULL, NULL, 0);
	}

	if (!PBG(classes)) {
		ALLOC_HASHTABLE(PBG(classes));
		zend_hash_init(PBG(classes), 0, NULL, NULL, 0);
	}

	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(protocolbuffers)
{
#ifdef ZTS
		ts_free_id(pb_globals_id);
#else
		pb_globals_dtor(&php_pb_globals TSRMLS_CC);
#endif

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(protocolbuffers)
{
	if (PBG(messages)) {
		zend_try {
			int i = 0;
			HashPosition pos;
			pb_scheme_container **element;

			for(zend_hash_internal_pointer_reset_ex(PBG(messages), &pos);
							zend_hash_get_current_data_ex(PBG(messages), (void **)&element, &pos) == SUCCESS;
							zend_hash_move_forward_ex(PBG(messages), &pos)
			) {

				for (i = 0; i < (*element)->size; i++) {
					if ((*element)->scheme[i].name != NULL) {
						efree((*element)->scheme[i].name);
					}
					if ((*element)->scheme[i].mangled_name != NULL) {
						efree((*element)->scheme[i].mangled_name);
					}
				}

				if ((*element)->single_property_name != NULL) {
					efree((*element)->single_property_name);
				}

				if ((*element)->scheme != NULL) {
					efree((*element)->scheme);
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
			int i = 0;
			HashPosition pos;
			zend_class_entry **ce;

			zend_hash_destroy(PBG(classes));
			FREE_HASHTABLE(PBG(classes));
			PBG(classes) = NULL;
		}
		zend_end_try();
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
	PHP_PROTOCOLBUFFERS_EXTVER,
#endif
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_PROTOCOLBUFFERS
ZEND_GET_MODULE(protocolbuffers)
#endif
