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
#include "descriptor_builder.h"
#include "message_options.h"
#include "descriptor.h"

int php_pb_field_descriptor_get_property(HashTable *hash, const char *name, size_t name_len, zval **result TSRMLS_DC)
{
	char *key;
	int key_len;
	zval **resval;

	zend_mangle_property_name(&key, &key_len, "*", 1, (char*)name, name_len, 0);
	if (zend_hash_find(hash, key, key_len, (void **)&resval) == SUCCESS) {
		*result = *resval;
	}

	efree(key);
	return 0;
}


static void php_protocolbuffers_descriptor_builder_free_storage(zend_object *object TSRMLS_DC)
{
	zend_object_std_dtor(object TSRMLS_CC);
	efree(object);
}

zend_object_value php_protocolbuffers_descriptor_builder_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	zend_object *object;

	object = (zend_object*)ecalloc(1, sizeof(*object));
	zend_object_std_init(object, ce TSRMLS_CC);

#if ZEND_MODULE_API_NO >= 20100525
	object_properties_init(object, ce);
#else
	{
		zval *tmp;
		zend_hash_copy(object->properties, &ce->default_properties, (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));
	}
#endif
	retval.handle = zend_objects_store_put(object,
		(zend_objects_store_dtor_t)zend_objects_destroy_object,
		(zend_objects_free_object_storage_t) php_protocolbuffers_descriptor_builder_free_storage,
	NULL TSRMLS_CC);
	retval.handlers = zend_get_std_object_handlers();

	return retval;
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_add_field, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, field)
	ZEND_ARG_INFO(0, force_add)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_get_name, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_set_name, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_build, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_set_extension, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_add_extension_range, 0, 0, 2)
	ZEND_ARG_INFO(0, begin)
	ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_get_options, 0, 0, 0)
ZEND_END_ARG_INFO()

/* {{{ proto ProtocolBuffers_DescriptorBuilder ProtocolBuffers_DescriptorBuilder::__construct()
*/
PHP_METHOD(protocolbuffers_descriptor_builder, __construct)
{
	zval *instance = getThis();
	HashTable *properties = NULL;
	zval *tmp = NULL;

	ALLOC_HASHTABLE(properties);
	zend_hash_init(properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	MAKE_STD_ZVAL(tmp);
	ZVAL_NULL(tmp);
	zend_hash_update(properties, "name", sizeof("name"), (void **)&tmp, sizeof(zval), NULL);

	MAKE_STD_ZVAL(tmp);
	array_init(tmp);
	zend_hash_update(properties, "fields", sizeof("fields"), (void **)&tmp, sizeof(zval), NULL);

	MAKE_STD_ZVAL(tmp);
	object_init_ex(tmp, protocol_buffers_message_options_class_entry);
	php_protocolbuffers_message_options_init_properties(tmp TSRMLS_CC);
	zend_hash_update(properties, "options", sizeof("options"), (void **)&tmp, sizeof(zval), NULL);

	MAKE_STD_ZVAL(tmp);
	array_init(tmp);
	zend_hash_update(properties, "extension_ranges", sizeof("extension_ranges"), (void **)&tmp, sizeof(zval), NULL);

	zend_merge_properties(instance, properties, 1 TSRMLS_CC);
}
/* }}} */

/* {{{ proto void ProtocolBuffersDescriptorBuilder::addField(int $index, ProtocolBuffers_DescriptorField $field)
*/
PHP_METHOD(protocolbuffers_descriptor_builder, addField)
{
	zval *instance = getThis();
	zval *field = NULL;
	zval **fields = NULL;
	long index;
	zend_bool force_add = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"lO|b", &index, &field, protocol_buffers_field_descriptor_class_entry, &force_add) == FAILURE) {
		return;
	}

	if (index < 1 || index > ktagmax) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "index should be in %d - %d", 1, ktagmax);
		return;
	}

	if (zend_hash_find(Z_OBJPROP_P(instance), "fields", sizeof("fields"), (void **)&fields) == SUCCESS) {
		if (zend_hash_index_exists(Z_ARRVAL_PP(fields), index)) {
			if (force_add < 1) {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "tag number `%d` has already registered.", index);
			}
			return;
		}

		Z_ADDREF_P(field);
		zend_hash_index_update(Z_ARRVAL_PP(fields), index, (void**)&field, sizeof(zval *), NULL);
	}
}
/* }}} */

/* {{{ proto void ProtocolBuffersDescriptorBuilder::setName(string $name)
*/
PHP_METHOD(protocolbuffers_descriptor_builder, setName)
{
	zval *instance = getThis();
	char *name;
	long name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"s", &name, &name_len) == FAILURE) {
		return;
	}

	if (name_len > 0) {
		add_property_string_ex(instance, "name", sizeof("name"), name, 1 TSRMLS_CC);
	}
}
/* }}} */


/* {{{ proto string ProtocolBuffers_DescriptorBuilder::getName()
*/
PHP_METHOD(protocolbuffers_descriptor_builder, getName)
{
	zval *name;

	name = zend_read_property(protocol_buffers_descriptor_builder_class_entry, getThis(), "name", sizeof("name")-1, 0 TSRMLS_CC);
	RETURN_STRINGL(Z_STRVAL_P(name), Z_STRLEN_P(name), 1);
}
/* }}} */

/* {{{ proto ProtocolBuffersDescriptor ProtocolBuffersDescriptorBuilder::build()
*/
PHP_METHOD(protocolbuffers_descriptor_builder, build)
{
	zval *result, *name, *fields = NULL;
	php_protocolbuffers_descriptor *descriptor;

	MAKE_STD_ZVAL(result);
	object_init_ex(result, protocol_buffers_descriptor_class_entry);
	php_pb_descriptor_properties_init(result TSRMLS_CC);
	descriptor = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_descriptor, result);

	name = zend_read_property(protocol_buffers_descriptor_builder_class_entry, getThis(), "name", sizeof("name")-1, 0 TSRMLS_CC);
	if (Z_TYPE_P(name) == IS_STRING) {
		descriptor->name_len = Z_STRLEN_P(name);
		if (descriptor->name_len > 0) {
			descriptor->name = (char*)emalloc(descriptor->name_len+1);
			memset(descriptor->name, '\0', descriptor->name_len+1);
			memcpy(descriptor->name, Z_STRVAL_P(name), descriptor->name_len);
		}
	}

	fields = zend_read_property(protocol_buffers_descriptor_builder_class_entry, getThis(), "fields", sizeof("fields")-1, 0 TSRMLS_CC);
	if (fields != NULL && Z_TYPE_P(fields) == IS_ARRAY) {
		HashTable *proto;
		HashPosition pos;
		zval **element;
		int n;
		size_t sz;
		pb_scheme *ischeme;

		proto = Z_ARRVAL_P(fields);
		sz = zend_hash_num_elements(proto);

		ischeme = (pb_scheme*)emalloc(sizeof(pb_scheme) * sz);
		memset(ischeme, '\0', sizeof(pb_scheme) * sz);
		descriptor->container->size = sz;
		descriptor->container->scheme = ischeme;

		for(n = 0, zend_hash_internal_pointer_reset_ex(proto, &pos);
						zend_hash_get_current_data_ex(proto, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(proto, &pos), n++
		) {
			zval *tmp = NULL;
			int tsize = 0;

			ischeme[n].is_extension = 0;
			ischeme[n].tag = (int)pos->h;
			php_pb_field_descriptor_get_property(Z_OBJPROP_PP(element), "type", sizeof("type"), &tmp TSRMLS_CC);
			if (Z_TYPE_P(tmp) == IS_LONG) {
				ischeme[n].type = Z_LVAL_P(tmp);
			}

			php_pb_field_descriptor_get_property(Z_OBJPROP_PP(element), "name", sizeof("name"), &tmp TSRMLS_CC);
			if (Z_TYPE_P(tmp) == IS_STRING) {
				char *mangle;
				int mangle_len;

				tsize				  = Z_STRLEN_P(tmp)+1;

				ischeme[n].original_name		= (char*)emalloc(sizeof(char*) * tsize);
				ischeme[n].original_name_len	= tsize;

				memcpy(ischeme[n].original_name, Z_STRVAL_P(tmp), tsize);
				ischeme[n].original_name[tsize] = '\0';

				ischeme[n].name		= (char*)emalloc(sizeof(char*) * tsize);
				ischeme[n].name_len	= tsize;

				memcpy(ischeme[n].name, Z_STRVAL_P(tmp), tsize);
				ischeme[n].name[tsize] = '\0';
				php_strtolower(ischeme[n].name, tsize);
				ischeme[n].name_h = zend_inline_hash_func(ischeme[n].name, tsize);

				if (strcmp(ischeme[n].name, ischeme[n].original_name) == 0) {
					// use snake case?
					ischeme[n].magic_type = 0;
				} else {
					ischeme[n].magic_type = 1;
				}

				zend_mangle_property_name(&mangle, &mangle_len, (char*)"*", 1, (char*)ischeme[n].original_name, ischeme[n].original_name_len, 0);
				ischeme[n].mangled_name	 = mangle;
				ischeme[n].mangled_name_len = mangle_len;
				ischeme[n].mangled_name_h = zend_inline_hash_func(mangle, mangle_len);
				ischeme[n].skip = 0;
			}

			php_pb_field_descriptor_get_property(Z_OBJPROP_PP(element), "required", sizeof("required"), &tmp TSRMLS_CC);
			if (Z_TYPE_P(tmp) == IS_BOOL) {
				convert_to_long(tmp);
				ischeme[n].required = Z_LVAL_P(tmp);
			}

			php_pb_field_descriptor_get_property(Z_OBJPROP_PP(element), "optional", sizeof("optional"), &tmp TSRMLS_CC);
			if (Z_TYPE_P(tmp) == IS_BOOL) {
				convert_to_long(tmp);
				ischeme[n].optional = Z_LVAL_P(tmp);
			}

			php_pb_field_descriptor_get_property(Z_OBJPROP_PP(element), "repeated", sizeof("repeated"), &tmp TSRMLS_CC);
			if (Z_TYPE_P(tmp) == IS_BOOL) {
				convert_to_long(tmp);
				ischeme[n].repeated = Z_LVAL_P(tmp);
			}

			php_pb_field_descriptor_get_property(Z_OBJPROP_PP(element), "packable", sizeof("packable"), &tmp TSRMLS_CC);
			if (Z_TYPE_P(tmp) == IS_BOOL) {
				convert_to_long(tmp);
				ischeme[n].packed = Z_LVAL_P(tmp);
			}

			if (ischeme[n].type == TYPE_MESSAGE) {
				zend_class_entry **c;

				php_pb_field_descriptor_get_property(Z_OBJPROP_PP(element), "message", sizeof("message"), &tmp TSRMLS_CC);
				if (Z_TYPE_P(tmp) == IS_STRING) {
					if (zend_lookup_class(Z_STRVAL_P(tmp), Z_STRLEN_P(tmp), &c TSRMLS_CC) == FAILURE) {
						efree(result);
						zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "the class %s does not find.", Z_STRVAL_P(tmp));
						return;
					}

					ischeme[n].ce = *c;
				} else {
					efree(result);
					zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "message wiretype set. we need message parameter for referencing class entry.");
					return;
				}
			}
		}
	}

	/* process options */
	{
		zval *tmp = NULL;

		tmp  = zend_read_property(protocol_buffers_descriptor_builder_class_entry, getThis(), "options", sizeof("options")-1, 0 TSRMLS_CC);

		if (Z_TYPE_P(tmp) == IS_OBJECT) {
			zval *ext;
			ext = zend_read_property(protocol_buffers_descriptor_builder_class_entry, tmp, "extensions", sizeof("extensions")-1, 0 TSRMLS_CC);

			if (Z_TYPE_P(ext) == IS_ARRAY) {
				HashPosition pos;
				zval **element;

				for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(ext), &pos);
								zend_hash_get_current_data_ex(Z_ARRVAL_P(ext), (void **)&element, &pos) == SUCCESS;
								zend_hash_move_forward_ex(Z_ARRVAL_P(ext), &pos)
				) {
					if (Z_OBJCE_PP(element) == protocol_buffers_php_message_options_class_entry) {
						zval *val;

						val = zend_read_property(protocol_buffers_php_message_options_class_entry, *element, "use_single_property", sizeof("use_single_property")-1, 0 TSRMLS_CC);
						if (Z_TYPE_P(val) == IS_BOOL) {
							descriptor->container->use_single_property = Z_LVAL_P(val);
						}

						val = zend_read_property(protocol_buffers_php_message_options_class_entry, *element, "use_wakeup_and_sleep", sizeof("use_wakeup_and_sleep")-1, 0 TSRMLS_CC);
						if (Z_TYPE_P(val) == IS_BOOL) {
							descriptor->container->use_wakeup_and_sleep = Z_LVAL_P(val);
						}

						if (descriptor->container->use_single_property > 0) {
							val = zend_read_property(protocol_buffers_php_message_options_class_entry, *element, "single_property_name", sizeof("single_property_name")-1, 0 TSRMLS_CC);
							efree(descriptor->container->single_property_name);

							zend_mangle_property_name(&(descriptor->container->single_property_name), &(descriptor->container->single_property_name_len), (char*)"*", 1, (char*)Z_STRVAL_P(val), Z_STRLEN_P(val), 0);
							descriptor->container->single_property_h = zend_inline_hash_func(descriptor->container->single_property_name, descriptor->container->single_property_name_len+1);

							if (memcmp(descriptor->container->orig_single_property_name, Z_STRVAL_P(val), Z_STRLEN_P(val)) != 0) {
								descriptor->container->orig_single_property_name = emalloc(sizeof(char*) * Z_STRLEN_P(val));
								memcpy(descriptor->container->orig_single_property_name, Z_STRVAL_P(val), Z_STRLEN_P(val));
								descriptor->container->orig_single_property_name[Z_STRLEN_P(val)] = '\0';
								descriptor->container->orig_single_property_name_len = Z_STRLEN_P(val)+1;
							}
						}

						val = zend_read_property(protocol_buffers_php_message_options_class_entry, *element, "process_unknown_fields", sizeof("process_unknown_fields")-1, 0 TSRMLS_CC);
						if (Z_TYPE_P(val) == IS_BOOL) {
							descriptor->container->process_unknown_fields = Z_LVAL_P(val);
						}
					}
				}

			}
		}
	}

	{/* process extension ranges */
		zval **entry = NULL, **tmp = NULL;
		pb_extension_range *ranges;
		HashPosition pos;
		int i = 0;

		if (zend_hash_find(Z_OBJPROP_P(getThis()), "extension_ranges", sizeof("extension_ranges"), (void **)&tmp) == SUCCESS) {
			descriptor->container->extension_cnt = zend_hash_num_elements(Z_ARRVAL_PP(tmp));

			ranges = (pb_extension_range*)emalloc(sizeof(pb_extension_range) * descriptor->container->extension_cnt);
			memset(ranges, '\0', sizeof(pb_extension_range) * descriptor->container->extension_cnt);

			descriptor->container->extensions = ranges;
			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(tmp), &pos);
			while (zend_hash_get_current_data_ex(Z_ARRVAL_PP(tmp), (void **)&entry, &pos) == SUCCESS) {
				zval **value = NULL;

				if (zend_hash_find(Z_ARRVAL_PP(entry), "begin", sizeof("begin"), (void **)&value) == SUCCESS) {
					ranges[i].begin = Z_LVAL_PP(value);
				}

				if (zend_hash_find(Z_ARRVAL_PP(entry), "end", sizeof("end"), (void **)&value) == SUCCESS) {
					ranges[i].end = Z_LVAL_PP(value);
				}

				zend_hash_move_forward_ex(Z_ARRVAL_PP(tmp), &pos);
				i++;
			}

		}
	}

	if (descriptor->container->size > 0) {
		int n = 0;
		pb_scheme *ischeme;
		zval *arrval = NULL;
		char *property = {0};
		int property_len = 0;
		MAKE_STD_ZVAL(arrval);
		array_init(arrval);

		for (n = 0; n < descriptor->container->size; n++) {
			zval *tmp = NULL, *value = NULL;
			char *name = {0};
			int name_length = 0;

			ischeme = &(descriptor->container->scheme[n]);

			MAKE_STD_ZVAL(tmp);
			object_init_ex(tmp, protocol_buffers_field_descriptor_class_entry);

			zend_mangle_property_name(&name, &name_length, (char*)"*", 1, (char*)"name", sizeof("name"), 0);
			MAKE_STD_ZVAL(value);
			ZVAL_STRING(value, ischeme->name, 1);
			zend_hash_update(Z_OBJPROP_P(tmp), name, name_length, (void **)&value, sizeof(zval*), NULL);
			efree(name);

			zend_mangle_property_name(&name, &name_length, (char*)"*", 1, (char*)"type", sizeof("type"), 0);
			MAKE_STD_ZVAL(value);
			ZVAL_LONG(value, ischeme->type);
			zend_hash_update(Z_OBJPROP_P(tmp), name, name_length, (void **)&value, sizeof(zval*), NULL);
			efree(name);

			zend_mangle_property_name(&name, &name_length, (char*)"*", 1, (char*)"extension", sizeof("extension"), 0);
			MAKE_STD_ZVAL(value);
			ZVAL_BOOL(value, ischeme->is_extension);
			zend_hash_update(Z_OBJPROP_P(tmp), name, name_length, (void **)&value, sizeof(zval*), NULL);
			efree(name);

			zend_hash_index_update(Z_ARRVAL_P(arrval), ischeme->tag, (void *)&tmp, sizeof(zval *), NULL);
		}

		zend_mangle_property_name(&property, &property_len, (char*)"*", 1, (char*)"fields", sizeof("fields"), 0);
		zend_hash_update(Z_OBJPROP_P(result), property, property_len, (void **)&arrval, sizeof(zval *), NULL);
		efree(property);
	}

	RETURN_ZVAL(result, 0, 1);
}
/* }}} */

/* {{{ proto ProtocolBuffersMessageOptions ProtocolBuffersDescriptorBuilder::getOptions()
*/
PHP_METHOD(protocolbuffers_descriptor_builder, getOptions)
{
	zval *options = NULL;
	options = zend_read_property(protocol_buffers_descriptor_builder_class_entry, getThis(), "options", sizeof("options")-1, 0 TSRMLS_CC);
	RETURN_ZVAL(options, 1, 0);
}
/* }}} */


/* {{{ proto ProtocolBuffersMessageOptions ProtocolBuffersDescriptorBuilder::addExtensionRange(int $begin, int $end)
*/
PHP_METHOD(protocolbuffers_descriptor_builder, addExtensionRange)
{
	zval *instance = getThis();
	zval *z_begin = NULL, *z_end = NULL, **fields = NULL, **extension_ranges = NULL;
	long begin = 0, end = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"ll", &begin, &end) == FAILURE) {
		return;
	}

	if (begin == end || begin > end) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "range end must be bigger than range begin.");
		return;
	}
	if (end > 536870912) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "range end must be smaller than %d", 536870912);
		return;
	}
	if (begin < 1 || end < 1) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "range must be greater than zero");
		return;
	}


	if (zend_hash_find(Z_OBJPROP_P(instance), "fields", sizeof("fields"), (void **)&fields) == SUCCESS) {
		HashPosition pos = 0;
		char *string_key = {0};
		uint string_key_len = 0;
		ulong num_key = 0;
		zval **entry = NULL;

		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(fields), &pos);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_PP(fields), (void **)&entry, &pos) == SUCCESS) {
			switch (zend_hash_get_current_key_ex(Z_ARRVAL_PP(fields), &string_key, &string_key_len, &num_key, 1, &pos)) {
				case HASH_KEY_IS_STRING:
				break;
				case HASH_KEY_IS_LONG:
					if (begin <= num_key && num_key <= end) {
						zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "extension range must be greater than existing tag number");
						return;
					}
				break;
			}

			zend_hash_move_forward_ex(Z_ARRVAL_PP(fields), &pos);
		}
	}

	if (zend_hash_find(Z_OBJPROP_P(instance), "extension_ranges", sizeof("extension_ranges"), (void **)&extension_ranges) == SUCCESS) {
		zval *array = NULL;
		MAKE_STD_ZVAL(array);
		array_init(array);

		MAKE_STD_ZVAL(z_begin);
		MAKE_STD_ZVAL(z_end);
		ZVAL_LONG(z_begin, begin);
		ZVAL_LONG(z_end, end);

		zend_hash_update(Z_ARRVAL_P(array), "begin", sizeof("begin"), (void **)&z_begin, sizeof(zval*), NULL);
		zend_hash_update(Z_ARRVAL_P(array), "end",   sizeof("end"),   (void **)&z_end, sizeof(zval*), NULL);

		zend_hash_next_index_insert(Z_ARRVAL_PP(extension_ranges), &array, sizeof(zval *), NULL);
	}
}
/* }}} */

static zend_function_entry php_protocolbuffers_descriptor_builder_methods[] = {
	PHP_ME(protocolbuffers_descriptor_builder, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(protocolbuffers_descriptor_builder, addField,  arginfo_pb_descriptor_builder_add_field, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_descriptor_builder, getName,   arginfo_pb_descriptor_builder_get_name, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_descriptor_builder, setName,   arginfo_pb_descriptor_builder_set_name, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_descriptor_builder, build,     arginfo_pb_descriptor_builder_build, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_descriptor_builder, addExtensionRange, arginfo_pb_descriptor_builder_add_extension_range, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_descriptor_builder, getOptions, arginfo_pb_descriptor_builder_get_options, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void php_pb_descriptor_builder_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersDescriptorBuilder", php_protocolbuffers_descriptor_builder_methods);
	protocol_buffers_descriptor_builder_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	protocol_buffers_descriptor_builder_class_entry->create_object = php_protocolbuffers_descriptor_builder_new;

	zend_declare_property_null(protocol_buffers_descriptor_builder_class_entry, "name", sizeof("name")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(protocol_buffers_descriptor_builder_class_entry, "fields", sizeof("fields")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(protocol_buffers_descriptor_builder_class_entry, "options", sizeof("options")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(protocol_buffers_descriptor_builder_class_entry, "extension_ranges", sizeof("extension_ranges")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "DescriptorBuilder", protocol_buffers_descriptor_builder_class_entry);
}
