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

#include "php_protocol_buffers.h"
#include "unknown_field_set.h"

static void php_protocolbuffers_unknown_field_set_free_storage(php_protocolbuffers_unknown_field_set *object TSRMLS_DC)
{
	zend_object_std_dtor(&object->zo TSRMLS_CC);
	efree(object);
}

zend_object_value php_protocol_buffers_unknown_field_set_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	PHP_PROTOCOLBUFFERS_STD_CREATE_OBJECT(php_protocolbuffers_unknown_field_set);

	object->max    = 0;
	object->offset = 0;

	return retval;
}


void php_pb_unknown_field_set_properties_init(zval *object TSRMLS_DC)
{
	HashTable *properties = NULL;
	zval *fields = NULL;

	ALLOC_HASHTABLE(properties);
	zend_hash_init(properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	MAKE_STD_ZVAL(fields);
	array_init(fields);
	zend_hash_update(properties, "fields", sizeof("fields"), (void **)&fields, sizeof(zval), NULL);
	zend_merge_properties(object, properties, 1 TSRMLS_CC);
}

void php_pb_unknown_field_clear(INTERNAL_FUNCTION_PARAMETERS, zval *instance)
{
	zval *fields = NULL, **prior_fields = NULL;
	char *name = {0};
	int name_len = 0;

	MAKE_STD_ZVAL(fields);
	array_init(fields);

	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"fields", sizeof("fields"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&prior_fields) == SUCCESS) {
		zval_ptr_dtor(prior_fields);
		prior_fields = NULL;
	}

	zend_hash_update(Z_OBJPROP_P(instance), name, name_len, (void **)&fields, sizeof(zval *), NULL);
	efree(name);
}

int php_pb_unknown_field_get_field(INTERNAL_FUNCTION_PARAMETERS, zval *instance, int number, char *name, int name_len, php_protocolbuffers_unknown_field **f)
{
	zval **element = NULL, **fields = NULL;
	HashPosition pos;
	HashTable *fieldht = NULL;
	int found = 0;

	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"fields", sizeof("fields"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&fields) == SUCCESS) {

		fieldht = Z_ARRVAL_PP(fields);
		for(zend_hash_internal_pointer_reset_ex(fieldht, &pos);
						zend_hash_get_current_data_ex(fieldht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(fieldht, &pos)
		) {
			php_protocolbuffers_unknown_field *t = NULL;

			if (Z_TYPE_PP(element) == IS_OBJECT) {
				t = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, *element);
				if (t->number == number) {
					*f = t;
					found = 1;
					break;
				}
			}
		}
	}

	efree(name);
	return found;
}


void php_pb_unknown_field_set_add_field(INTERNAL_FUNCTION_PARAMETERS, zval *instance, int number, char *name, int name_len, zval *field)
{
	zval **fields = NULL;

	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"fields", sizeof("fields"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&fields) == SUCCESS) {
/* NOTE: we allow duplicate number here.
		fieldht = Z_ARRVAL_PP(fields);

		for(zend_hash_internal_pointer_reset_ex(fieldht, &pos);
						zend_hash_get_current_data_ex(fieldht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(fieldht, &pos)
		) {
			php_protocolbuffers_unknown_field *s = NULL, *t = NULL;

			s = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, field);
			t = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, *element);

			if (t->number == number) {
				//efree(name);
				//zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "number %d has already added", number);
				return;
			}
		}
*/
	}

	zend_hash_next_index_insert(Z_ARRVAL_PP(fields), (void *)&field, sizeof(zval *), NULL);
	efree(name);
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_set___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_set_count, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_get_field, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_add_field, 0, 0, 1)
	ZEND_ARG_INFO(0, unknown_field)
ZEND_END_ARG_INFO()

/* {{{ proto void ProtocolBuffersUnknownFieldSet::count()
*/
PHP_METHOD(protocolbuffers_unknown_field_set, count)
{
	zval **fields = NULL, *instance = getThis();
	char *name = {0};
	int num = 0, name_len = 0;

	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"fields", sizeof("fields"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&fields) == SUCCESS) {
		num = zend_hash_num_elements(Z_ARRVAL_PP(fields));
	}
	efree(name);

	RETURN_LONG(num);
}
/* }}} */

/* {{{ proto ProtocolBuffersUnknownField ProtocolBuffersUnknownFieldSet::getField(int $index)
*/
PHP_METHOD(protocolbuffers_unknown_field_set, getField)
{
	zval **element = NULL, **fields = NULL, *result = NULL, *instance = getThis();
	HashTable *fieldht = NULL;
	HashPosition pos;
	char *name = {0};
	long tag = 0;
	int name_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &tag) == FAILURE) {
		return;
	}

	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"fields", sizeof("fields"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&fields) == SUCCESS) {
		fieldht = Z_ARRVAL_PP(fields);

		for(zend_hash_internal_pointer_reset_ex(fieldht, &pos);
						zend_hash_get_current_data_ex(fieldht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(fieldht, &pos)
		) {
			php_protocolbuffers_unknown_field *t = NULL;
			t = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, *element);

			if (t->number == tag) {
				result = *element;
				break;
			}
		}
	}
	efree(name);

	if (result != NULL) {
		RETURN_ZVAL(result, 1, 0);
	}
}
/* }}} */

/* {{{ proto void ProtocolBuffersUnknownFieldSet::addField(ProtocolBuffersUnknownField $field)
*/
PHP_METHOD(protocolbuffers_unknown_field_set, addField)
{
	zval *field = NULL, *instance = getThis();
	php_protocolbuffers_unknown_field *unknown_field = NULL;
	char *name = {0};
	int name_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"O", &field, protocol_buffers_unknown_field_class_entry) == FAILURE) {
		return;
	}

	unknown_field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, field);
	php_pb_unknown_field_set_add_field(INTERNAL_FUNCTION_PARAM_PASSTHRU, instance, unknown_field->number, name, name_len, field);
}
/* }}} */


/* {{{ proto mixed ProtocolBuffersUnknownFieldSet::current()
*/
PHP_METHOD(protocolbuffers_unknown_field_set, current)
{
	zval **fields = NULL, **d = NULL, *instance = getThis();
	php_protocolbuffers_unknown_field_set *unknown_field_set;
	char *name = {0};
	int num = 0, name_len = 0;

	unknown_field_set = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field_set, instance);
	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"fields", sizeof("fields"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&fields) == SUCCESS) {
		num = zend_hash_num_elements(Z_ARRVAL_PP(fields));
	}
	efree(name);

	if (zend_hash_index_find(Z_ARRVAL_PP(fields), unknown_field_set->offset, (void **)&d) == SUCCESS) {
		RETURN_ZVAL(*d, 1, 0);
	}
}
/* }}} */

/* {{{ proto mixed ProtocolBuffersUnknownFieldSet::key()
*/
PHP_METHOD(protocolbuffers_unknown_field_set, key)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field_set *set = NULL;

	set = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field_set, instance);
	RETURN_LONG(set->offset);
}
/* }}} */

/* {{{ proto mixed ProtocolBuffersUnknownFieldSet::next()
*/
PHP_METHOD(protocolbuffers_unknown_field_set, next)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field_set *set = NULL;

	set = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field_set, instance);
	set->offset++;
}
/* }}} */

/* {{{ proto void ProtocolBuffersUnknownFieldSet::rewind()
*/
PHP_METHOD(protocolbuffers_unknown_field_set, rewind)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field_set *set = NULL;

	set = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field_set, instance);
	set->offset = 0;
}
/* }}} */

/* {{{ proto bool ProtocolBuffersUnknownFieldSet::valid()
*/
PHP_METHOD(protocolbuffers_unknown_field_set, valid)
{
	zval **fields = NULL, *instance = getThis();
	php_protocolbuffers_unknown_field_set *set = NULL;
	char *name = {0};
	int num = 0, name_len = 0;

	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"fields", sizeof("fields"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&fields) == SUCCESS) {
		num = zend_hash_num_elements(Z_ARRVAL_PP(fields));
	}
	efree(name);

	set = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field_set, instance);
	if (-1 < set->offset && set->offset < num) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

static zend_function_entry php_protocolbuffers_unknown_field_set_methods[] = {
	PHP_ME(protocolbuffers_unknown_field_set, count, arginfo_pb_unknown_field_set_count, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field_set, getField, arginfo_pb_unknown_field_get_field, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field_set, addField, arginfo_pb_unknown_field_add_field, ZEND_ACC_PUBLIC)
	/* iterator */
	PHP_ME(protocolbuffers_unknown_field_set, current, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field_set, key,     NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field_set, next,    NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field_set, rewind,  NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field_set, valid,   NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void php_pb_unknown_field_set_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersUnknownFieldSet", php_protocolbuffers_unknown_field_set_methods);
	protocol_buffers_unknown_field_set_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(protocol_buffers_unknown_field_set_class_entry TSRMLS_CC, 1, zend_ce_iterator);
	protocol_buffers_unknown_field_set_class_entry->create_object = php_protocol_buffers_unknown_field_set_new;

	zend_declare_property_null(protocol_buffers_unknown_field_set_class_entry, "fields", sizeof("fields")-1, ZEND_ACC_PROTECTED TSRMLS_CC);

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "UnknownFieldSet", protocol_buffers_unknown_field_set_class_entry);
}
