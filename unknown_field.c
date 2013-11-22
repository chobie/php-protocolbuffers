#include "php_protocol_buffers.h"
#include "unknown_field.h"

static zend_object_handlers php_protocolbuffers_unknown_field_object_handlers;


void php_pb_unknown_field_properties_init(zval *object TSRMLS_DC)
{
#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 3)
	HashTable *properties;
	zval *pp = NULL;
	ALLOC_HASHTABLE(properties);
	zend_hash_init(properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	MAKE_STD_ZVAL(pp);
	ZVAL_NULL(pp);
	zend_hash_update(properties, "number", sizeof("number"), (void **)&pp, sizeof(zval), NULL);
	MAKE_STD_ZVAL(pp);
	ZVAL_NULL(pp);
	zend_hash_update(properties, "type", sizeof("type"), (void **)&pp, sizeof(zval), NULL);

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

void php_pb_unknown_field_set_number(zval *instance, int number TSRMLS_DC)
{
	php_protocolbuffers_unknown_field *field = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	field->number = number;
}

void php_pb_unknown_field_set_type(zval *instance, int type TSRMLS_DC)
{
	php_protocolbuffers_unknown_field *field = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	field->type = type;
}

#else

void php_pb_unknown_field_set_number(zval *instance, int number TSRMLS_DC)
{
	php_protocolbuffers_unknown_field *field = NULL;
	char *name;
	int name_len;
	zval **result = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"number", sizeof("number"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&result) == SUCCESS) {
		zval *tmp;

		MAKE_STD_ZVAL(tmp);
		ZVAL_LONG(tmp, number);
		zend_hash_update(Z_OBJPROP_P(instance), name, name_len, (void **)&tmp, sizeof(zval), NULL);
		field->number = number;
	}
	efree(name);
}

void php_pb_unknown_field_set_type(zval *instance, int type TSRMLS_DC)
{
	php_protocolbuffers_unknown_field *field = NULL;
	char *name;
	int name_len;
	zval **result = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"type", sizeof("type"), 0);
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

zend_object_value php_protocolbuffers_unknown_field_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	PHP_PROTOCOLBUFFERS_STD_CREATE_OBJECT(php_protocolbuffers_unknown_field);

	ALLOC_HASHTABLE(object->ht);
	zend_hash_init(object->ht, 0, NULL, NULL, 0);

	retval.handlers = &php_protocolbuffers_unknown_field_object_handlers;

	return retval;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_set_number, 0, 0, 1)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_set_type, 0, 0, 1)
	ZEND_ARG_INFO(0, wiretype)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_get_number, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_get_type, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_get_as_varint, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_get_as_length_delimited, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_get_length_delimited_size, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_get_as_fixed32, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_get_as_fixed64, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_get_as_float_list, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_get_as_double_list, 0, 0, 0)
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
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;
	unknown_value **element;
	HashPosition pos;
	zval *result;

	MAKE_STD_ZVAL(result);
	array_init(result);
	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);

	if (field->type != WIRETYPE_VARINT) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "wiretype mismatched. expected varint. but %d", field->type);
		return;
	}

	for(zend_hash_internal_pointer_reset_ex(field->ht, &pos);
						zend_hash_get_current_data_ex(field->ht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(field->ht, &pos)
		) {
		zval *tmp;
		pbf __payload;

		MAKE_STD_ZVAL(tmp);
		__payload.type = TYPE_INT64;__payload.value.int64 = (int64_t)(*element)->varint;
		pb_format_string(tmp, &__payload TSRMLS_CC);

		zend_hash_next_index_insert(Z_ARRVAL_P(result), &tmp, sizeof(zval *), NULL);
	}

	RETURN_ZVAL(result, 0, 1);
}
/* }}} */

/* {{{ proto string ProtocolBuffersUnknownField::getAsLengthDelimitedList()
*/
PHP_METHOD(protocolbuffers_unknown_field, getAsLengthDelimitedList)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;
	unknown_value **element;
	HashPosition pos;
	zval *result;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	if (field->type == WIRETYPE_VARINT) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "wiretype mismatched. expected varint. but %d", field->type);
	}

	MAKE_STD_ZVAL(result);
	array_init(result);
	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	for(zend_hash_internal_pointer_reset_ex(field->ht, &pos);
						zend_hash_get_current_data_ex(field->ht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(field->ht, &pos)
		) {
		zval *tmp;
		MAKE_STD_ZVAL(tmp);

		ZVAL_STRINGL(tmp, (const char*)(*element)->buffer.val,  (*element)->buffer.len, 1);
		zend_hash_next_index_insert(Z_ARRVAL_P(result), &tmp, sizeof(zval *), NULL);
	}

	RETURN_ZVAL(result, 0, 1);
}
/* }}} */

/* {{{ proto long ProtocolBuffersUnknownField::getAsFixed32List()
*/
PHP_METHOD(protocolbuffers_unknown_field, getAsFixed32List)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;
	unknown_value **element;
	HashPosition pos;
	zval *result;


	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	if (field->type == WIRETYPE_VARINT) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "wiretype mismatched. expected varint. but %d", field->type);
	}

	MAKE_STD_ZVAL(result);
	array_init(result);
	for(zend_hash_internal_pointer_reset_ex(field->ht, &pos);
						zend_hash_get_current_data_ex(field->ht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(field->ht, &pos)
		) {
		uint32_t fixed;
		zval *tmp;
		pbf __payload;
		MAKE_STD_ZVAL(tmp);
		memcpy(&fixed, (*element)->buffer.val, (*element)->buffer.len);

		__payload.type = TYPE_FIXED32;__payload.value.uint32 = (uint32_t)(*element)->varint;
		pb_format_string(tmp, &__payload TSRMLS_CC);
		zend_hash_next_index_insert(Z_ARRVAL_P(result), &tmp, sizeof(zval *), NULL);
	}

	RETURN_ZVAL(result, 0, 1);
}
/* }}} */

/* {{{ proto long ProtocolBuffersUnknownField::getAsFixed64List()
*/
PHP_METHOD(protocolbuffers_unknown_field, getAsFixed64List)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;
	unknown_value **element;
	HashPosition pos;
	zval *result;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	if (field->type == WIRETYPE_VARINT) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "wiretype mismatched. expected varint. but %d", field->type);
	}

	MAKE_STD_ZVAL(result);
	array_init(result);
	for(zend_hash_internal_pointer_reset_ex(field->ht, &pos);
						zend_hash_get_current_data_ex(field->ht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(field->ht, &pos)
		) {
		uint64_t fixed;
		zval *tmp;
		pbf __payload;
		MAKE_STD_ZVAL(tmp);
		memcpy(&fixed, (*element)->buffer.val, (*element)->buffer.len);

		__payload.type = TYPE_FIXED64;__payload.value.uint64 = (uint64_t)(*element)->varint;
		pb_format_string(tmp, &__payload TSRMLS_CC);
		zend_hash_next_index_insert(Z_ARRVAL_P(result), &tmp, sizeof(zval *), NULL);
	}

	RETURN_ZVAL(result, 0, 1);
}
/* }}} */

/* {{{ proto long ProtocolBuffersUnknownField::getAsFloatList()
*/
PHP_METHOD(protocolbuffers_unknown_field, getAsFloatList)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;
	unknown_value **element;
	HashPosition pos;
	zval *result;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	if (field->type == WIRETYPE_VARINT) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "wiretype mismatched. expected varint. but %d", field->type);
	}

	MAKE_STD_ZVAL(result);
	array_init(result);
	for(zend_hash_internal_pointer_reset_ex(field->ht, &pos);
						zend_hash_get_current_data_ex(field->ht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(field->ht, &pos)
		) {
		union {
			uint32_t v;
			float f;
		} u;
		zval *tmp;
		pbf __payload;

		MAKE_STD_ZVAL(tmp);
		memcpy(&u.v, (*element)->buffer.val, sizeof(float));

		__payload.type = TYPE_FLOAT;__payload.value.f = (float)u.f;
		pb_format_string(tmp, &__payload TSRMLS_CC);
		zend_hash_next_index_insert(Z_ARRVAL_P(result), &tmp, sizeof(zval *), NULL);
	}

	RETURN_ZVAL(result, 0, 1);
}
/* }}} */

/* {{{ proto long ProtocolBuffersUnknownField::getAsDoubleList()
*/
PHP_METHOD(protocolbuffers_unknown_field, getAsDoubleList)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;
	unknown_value **element;
	HashPosition pos;
	zval *result;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	if (field->type == WIRETYPE_VARINT) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "wiretype mismatched. expected varint. but %d", field->type);
	}

	MAKE_STD_ZVAL(result);
	array_init(result);
	for(zend_hash_internal_pointer_reset_ex(field->ht, &pos);
						zend_hash_get_current_data_ex(field->ht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(field->ht, &pos)
		) {
		union {
			uint64_t v;
			double d;
		} u;

		zval *tmp;
		pbf __payload;
		MAKE_STD_ZVAL(tmp);
		memcpy(&u.v, (*element)->buffer.val, sizeof(double));

		__payload.type = TYPE_DOUBLE;__payload.value.d = (double)u.d;
		pb_format_string(tmp, &__payload TSRMLS_CC);
		zend_hash_next_index_insert(Z_ARRVAL_P(result), &tmp, sizeof(zval *), NULL);
	}

	RETURN_ZVAL(result, 0, 1);
}
/* }}} */



/* {{{ proto bool ProtocolBuffersUnknownField::isVarint()
*/
PHP_METHOD(protocolbuffers_unknown_field, isVarint)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	if (field->type == WIRETYPE_VARINT) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool ProtocolBuffersUnknownField::isFixed64()
*/
PHP_METHOD(protocolbuffers_unknown_field, isFixed64)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	if (field->type == WIRETYPE_FIXED64) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool ProtocolBuffersUnknownField::isLengthDelimited()
*/
PHP_METHOD(protocolbuffers_unknown_field, isLengthDelimited)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	if (field->type == WIRETYPE_LENGTH_DELIMITED) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool ProtocolBuffersUnknownField::isFixed32()
*/
PHP_METHOD(protocolbuffers_unknown_field, isFixed32)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	if (field->type == WIRETYPE_FIXED32) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


static zend_function_entry php_protocolbuffers_unknown_field_methods[] = {
	PHP_ME(protocolbuffers_unknown_field, getNumber, arginfo_pb_unknown_field_get_number, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getType, arginfo_pb_unknown_field_get_type, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, isVarint, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, isFixed64, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, isLengthDelimited, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, isFixed32, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsVarintList, arginfo_pb_unknown_field_get_as_varint, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsLengthDelimitedList, arginfo_pb_unknown_field_get_as_length_delimited, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsFloatList, arginfo_pb_unknown_field_get_as_float_list, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsDoubleList, arginfo_pb_unknown_field_get_as_float_list, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsFixed32List, arginfo_pb_unknown_field_get_as_fixed32, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsFixed64List, arginfo_pb_unknown_field_get_as_fixed64, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void php_pb_unknown_field_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersUnknownField", php_protocolbuffers_unknown_field_methods);
	protocol_buffers_unknown_field_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	protocol_buffers_unknown_field_class_entry->create_object = php_protocolbuffers_unknown_field_new;

	memcpy(&php_protocolbuffers_unknown_field_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2)
	php_protocolbuffers_unknown_field_object_handlers.get_debug_info = php_protocolbuffers_unknown_field_get_debug_info;
#else
	/* for var_dump. */
	zend_declare_property_long(protocol_buffers_unknown_field_class_entry, "number", sizeof("number")-1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(protocol_buffers_unknown_field_class_entry, "type", sizeof("type")-1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);
#endif

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "UnknownField", protocol_buffers_unknown_field_class_entry);
}
