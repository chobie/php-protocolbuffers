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
#include "unknown_field.h"

static zend_object_handlers php_protocolbuffers_unknown_field_object_handlers;


void php_protocolbuffers_unknown_field_properties_init(zval *object TSRMLS_DC)
{
#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 3)
	HashTable *properties;
	zval *number = NULL, *type = NULL;

	ALLOC_HASHTABLE(properties);
	zend_hash_init(properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	MAKE_STD_ZVAL(number);
	ZVAL_NULL(number);
	zend_hash_update(properties, "number", sizeof("number"), (void **)&number, sizeof(zval), NULL);

	MAKE_STD_ZVAL(type);
	ZVAL_NULL(type);
	zend_hash_update(properties, "type", sizeof("type"), (void **)&type, sizeof(zval), NULL);

	zend_merge_properties(object, properties, 1 TSRMLS_CC);
#endif
}


#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2)
static HashTable *php_protocolbuffers_unknown_field_get_debug_info(zval *obj, int *is_temp TSRMLS_DC)
{
	HashTable *ht;
	HashTable *props = Z_OBJPROP_P(obj);
	zval *number, *type, *count;
	php_protocolbuffers_unknown_field *field;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, obj);
	*is_temp = 1;

	ALLOC_HASHTABLE(ht);
	ZEND_INIT_SYMTABLE_EX(ht, zend_hash_num_elements(props), 0);

	MAKE_STD_ZVAL(number);
	MAKE_STD_ZVAL(type);
	MAKE_STD_ZVAL(count);

	ZVAL_LONG(number, field->number);
	ZVAL_LONG(type, field->type);
	ZVAL_LONG(count, zend_hash_num_elements(field->ht));

	zend_hash_update(ht, "number", sizeof("number"), (void **)&number, sizeof(zval *), NULL);
	zend_hash_update(ht, "type", sizeof("type"), (void **)&type, sizeof(zval *), NULL);
	zend_hash_update(ht, "count", sizeof("count"), (void **)&count, sizeof(zval *), NULL);

	return ht;
}

void php_protocolbuffers_unknown_field_set_number(zval *instance, int number TSRMLS_DC)
{
	php_protocolbuffers_unknown_field *field = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	field->number = number;
}

void php_protocolbuffers_unknown_field_set_type(zval *instance, int type TSRMLS_DC)
{
	php_protocolbuffers_unknown_field *field = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	field->type = type;
}

#else

void php_protocolbuffers_unknown_field_set_number(zval *instance, int number TSRMLS_DC)
{
	php_protocolbuffers_unknown_field *field = NULL;
	char *name = {0};
	int name_len = 0;
	zval **result = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)ZEND_STRS("number"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&result) == SUCCESS) {
		zval *tmp;

		MAKE_STD_ZVAL(tmp);
		ZVAL_LONG(tmp, number);
		zend_hash_update(Z_OBJPROP_P(instance), name, name_len, (void **)&tmp, sizeof(zval), NULL);
		field->number = number;
	}
	efree(name);
}

void php_protocolbuffers_unknown_field_set_type(zval *instance, int type TSRMLS_DC)
{
	php_protocolbuffers_unknown_field *field = NULL;
	char *name = {0};
	int name_len = 0;
	zval **result = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)ZEND_STRS("type"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&result) == SUCCESS) {
		zval *tmp;

		MAKE_STD_ZVAL(tmp);
		ZVAL_LONG(tmp, type);
		zend_hash_update(Z_OBJPROP_P(instance), name, name_len, (void **)&tmp, sizeof(zval), NULL);
		field->type = type;
	}
	efree(name);
}
#endif


static void php_protocolbuffers_unknown_field_free_storage(php_protocolbuffers_unknown_field *object TSRMLS_DC)
{
	HashPosition pos;
	unknown_value **element;

	for(zend_hash_internal_pointer_reset_ex(object->ht, &pos);
						zend_hash_get_current_data_ex(object->ht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(object->ht, &pos)
		) {
		switch (object->type) {
			case WIRETYPE_VARINT:
				efree(*element);
			break;
			case WIRETYPE_FIXED64:
			case WIRETYPE_FIXED32:
			case WIRETYPE_LENGTH_DELIMITED:
				if ((*element)->buffer.len > 0) {
					efree((*element)->buffer.val);
					(*element)->buffer.val = NULL;
					(*element)->buffer.len = 0;
				}
				efree(*element);
			break;
		}
	}

	zend_hash_destroy(object->ht);
	FREE_HASHTABLE(object->ht);
	object->ht = NULL;

	zend_object_std_dtor(&object->zo TSRMLS_CC);
	efree(object);
}

static void php_protocolbuffers_check_type_return(INTERNAL_FUNCTION_PARAMETERS, enum WireType type)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	if (field->type == type) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}

}

static void php_protocolbuffers_unknown_field_get_as(INTERNAL_FUNCTION_PARAMETERS, enum WireType type, enum FieldType field_type, int use_string)
{
	zval *tmp = NULL, *result = NULL, *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;
	unknown_value **element = NULL;
	pbf __payload;
	HashPosition pos;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);

	if (type != WIRETYPE_LENGTH_DELIMITED) {
		if (field->type != type) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "wiretype mismatched. expected %d. but %d", type, field->type);
			return;
		}
	}

	MAKE_STD_ZVAL(result);
	array_init(result);

	for(zend_hash_internal_pointer_reset_ex(field->ht, &pos);
						zend_hash_get_current_data_ex(field->ht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(field->ht, &pos)) {
		MAKE_STD_ZVAL(tmp);

		if (type == WIRETYPE_LENGTH_DELIMITED) {
			ZVAL_STRINGL(tmp, (const char*)(*element)->buffer.val,  (*element)->buffer.len, 1);
		} else {
			if (field_type == TYPE_DOUBLE) {
				union {
					uint64_t v;
					double d;
				} u;
				memcpy(&u.v, (*element)->buffer.val, sizeof(double));
				__payload.type = TYPE_DOUBLE;__payload.value.d = (double)u.d;
			} else if (field_type == TYPE_FLOAT) {
				union {
					uint32_t v;
					float f;
				} u;
				memcpy(&u.v, (*element)->buffer.val, sizeof(float));
				__payload.type = TYPE_FLOAT;__payload.value.f = (float)u.f;
			} else if (type == WIRETYPE_FIXED64) {
				uint64_t fixed64 = 0;

				memcpy(&fixed64, (*element)->buffer.val, (*element)->buffer.len);
				__payload.type = TYPE_FIXED64;__payload.value.uint64 = fixed64;
			} else if (type == WIRETYPE_FIXED32) {
				uint32_t fixed32 = 0;

				memcpy(&fixed32, (*element)->buffer.val, (*element)->buffer.len);
				__payload.type = TYPE_FIXED32;__payload.value.uint32 = fixed32;
			} else if (type == WIRETYPE_VARINT) {
				__payload.type = TYPE_INT64;__payload.value.int64 = (int64_t)(*element)->varint;
			} else {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "passed unhandled type wire_type:%d field_type:%d. this is bug", type, field_type);
				return;
			}

			php_protocolbuffers_format_string(tmp, &__payload, use_string TSRMLS_CC);
		}

		zend_hash_next_index_insert(Z_ARRVAL_P(result), &tmp, sizeof(zval *), NULL);
	}

	RETURN_ZVAL(result, 0, 1);
}

zend_object_value php_protocolbuffers_unknown_field_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	PHP_PROTOCOLBUFFERS_STD_CREATE_OBJECT(php_protocolbuffers_unknown_field);

	ALLOC_HASHTABLE(object->ht);
	zend_hash_init(object->ht, 0, NULL, NULL, 0);

	retval.handlers = &php_protocolbuffers_unknown_field_object_handlers;

	return retval;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_unknown_field_get_number, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_unknown_field_get_type, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_unknown_field_get_as_varint, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_unknown_field_get_as_length_delimited, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_unknown_field_get_length_delimited_size, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_unknown_field_get_as_fixed32, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_unknown_field_get_as_fixed64, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_unknown_field_get_as_float_list, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_unknown_field_get_as_double_list, 0, 0, 0)
ZEND_END_ARG_INFO()


/* {{{ proto void ProtocolBuffersUnknownField::getNumber()
*/
PHP_METHOD(protocolbuffers_unknown_field, getNumber)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	RETURN_LONG(field->number);
}
/* }}} */

/* {{{ proto void ProtocolBuffersUnknownField::getType()
*/
PHP_METHOD(protocolbuffers_unknown_field, getType)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	RETURN_LONG(field->type);
}
/* }}} */

/* {{{ proto long ProtocolBuffersUnknownField::getAsVarint()
*/
PHP_METHOD(protocolbuffers_unknown_field, getAsVarintList)
{
	php_protocolbuffers_unknown_field_get_as(INTERNAL_FUNCTION_PARAM_PASSTHRU, WIRETYPE_VARINT, -1, PBG(native_scalar));
}
/* }}} */

/* {{{ proto string ProtocolBuffersUnknownField::getAsLengthDelimitedList()
*/
PHP_METHOD(protocolbuffers_unknown_field, getAsLengthDelimitedList)
{
	php_protocolbuffers_unknown_field_get_as(INTERNAL_FUNCTION_PARAM_PASSTHRU, WIRETYPE_LENGTH_DELIMITED, -1, PBG(native_scalar));
}
/* }}} */

/* {{{ proto long ProtocolBuffersUnknownField::getAsFixed32List()
*/
PHP_METHOD(protocolbuffers_unknown_field, getAsFixed32List)
{
	php_protocolbuffers_unknown_field_get_as(INTERNAL_FUNCTION_PARAM_PASSTHRU, WIRETYPE_FIXED32, -1, PBG(native_scalar));
}
/* }}} */

/* {{{ proto long ProtocolBuffersUnknownField::getAsFixed64List()
*/
PHP_METHOD(protocolbuffers_unknown_field, getAsFixed64List)
{
	php_protocolbuffers_unknown_field_get_as(INTERNAL_FUNCTION_PARAM_PASSTHRU, WIRETYPE_FIXED64, -1, PBG(native_scalar));
}
/* }}} */

/* {{{ proto long ProtocolBuffersUnknownField::getAsFloatList()
*/
PHP_METHOD(protocolbuffers_unknown_field, getAsFloatList)
{
	php_protocolbuffers_unknown_field_get_as(INTERNAL_FUNCTION_PARAM_PASSTHRU, WIRETYPE_FIXED32, TYPE_FLOAT, PBG(native_scalar));
}
/* }}} */

/* {{{ proto long ProtocolBuffersUnknownField::getAsDoubleList()
*/
PHP_METHOD(protocolbuffers_unknown_field, getAsDoubleList)
{
	php_protocolbuffers_unknown_field_get_as(INTERNAL_FUNCTION_PARAM_PASSTHRU, WIRETYPE_FIXED64, TYPE_DOUBLE, PBG(native_scalar));
}
/* }}} */

/* {{{ proto bool ProtocolBuffersUnknownField::isVarint()
*/
PHP_METHOD(protocolbuffers_unknown_field, isVarint)
{
	php_protocolbuffers_check_type_return(INTERNAL_FUNCTION_PARAM_PASSTHRU, WIRETYPE_VARINT);
}
/* }}} */

/* {{{ proto bool ProtocolBuffersUnknownField::isFixed64()
*/
PHP_METHOD(protocolbuffers_unknown_field, isFixed64)
{
	php_protocolbuffers_check_type_return(INTERNAL_FUNCTION_PARAM_PASSTHRU, WIRETYPE_FIXED64);
}
/* }}} */

/* {{{ proto bool ProtocolBuffersUnknownField::isLengthDelimited()
*/
PHP_METHOD(protocolbuffers_unknown_field, isLengthDelimited)
{
	php_protocolbuffers_check_type_return(INTERNAL_FUNCTION_PARAM_PASSTHRU, WIRETYPE_LENGTH_DELIMITED);
}
/* }}} */

/* {{{ proto bool ProtocolBuffersUnknownField::isFixed32()
*/
PHP_METHOD(protocolbuffers_unknown_field, isFixed32)
{
	php_protocolbuffers_check_type_return(INTERNAL_FUNCTION_PARAM_PASSTHRU, WIRETYPE_FIXED32);
}
/* }}} */


static zend_function_entry php_protocolbuffers_unknown_field_methods[] = {
	PHP_ME(protocolbuffers_unknown_field, getNumber, arginfo_protocolbuffers_unknown_field_get_number, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getType, arginfo_protocolbuffers_unknown_field_get_type, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, isVarint, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, isFixed64, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, isLengthDelimited, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, isFixed32, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsVarintList, arginfo_protocolbuffers_unknown_field_get_as_varint, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsLengthDelimitedList, arginfo_protocolbuffers_unknown_field_get_as_length_delimited, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsFloatList, arginfo_protocolbuffers_unknown_field_get_as_float_list, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsDoubleList, arginfo_protocolbuffers_unknown_field_get_as_float_list, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsFixed32List, arginfo_protocolbuffers_unknown_field_get_as_fixed32, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsFixed64List, arginfo_protocolbuffers_unknown_field_get_as_fixed64, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void php_protocolbuffers_unknown_field_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersUnknownField", php_protocolbuffers_unknown_field_methods);
	php_protocol_buffers_unknown_field_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	php_protocol_buffers_unknown_field_class_entry->create_object = php_protocolbuffers_unknown_field_new;

	memcpy(&php_protocolbuffers_unknown_field_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2)
	php_protocolbuffers_unknown_field_object_handlers.get_debug_info = php_protocolbuffers_unknown_field_get_debug_info;
#else
	/* for var_dump. */
	zend_declare_property_long(php_protocol_buffers_unknown_field_class_entry, ZEND_STRS("number")-1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(php_protocol_buffers_unknown_field_class_entry, ZEND_STRS("type")-1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);
#endif

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "UnknownField", php_protocol_buffers_unknown_field_class_entry);
}
