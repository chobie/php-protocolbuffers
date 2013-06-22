#include "php_protocol_buffers.h"
#include "unknown_field.h"

static zend_object_handlers php_protocolbuffers_unknown_field_object_handlers;

#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2)
static HashTable *php_protocolbuffers_unknown_field_get_debug_info(zval *obj, int *is_temp TSRMLS_DC)
{
	HashTable *ht;
	HashTable *props = Z_OBJPROP_P(obj);
	zval *number, *type;
	php_protocolbuffers_unknown_field *field;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, obj);
	*is_temp = 1;

	ALLOC_HASHTABLE(ht);
	ZEND_INIT_SYMTABLE_EX(ht, zend_hash_num_elements(props), 0);

	MAKE_STD_ZVAL(number);
	MAKE_STD_ZVAL(type);

	ZVAL_LONG(number, field->number);
	ZVAL_LONG(type, field->type);

	zend_hash_update(ht, "number", sizeof("number"), (void **)&number, sizeof(zval *), NULL);
	zend_hash_update(ht, "type", sizeof("type"), (void **)&type, sizeof(zval *), NULL);

	return ht;
}
#endif

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


/* {{{ proto void ProtocolBuffersUnknownField::setNumber(int $number)
*/
PHP_METHOD(protocolbuffers_unknown_field, setNumber)
{
	long number = 0;
	zval *instance = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &number) == FAILURE) {
		return;
	}

	if (number < 1 || number > ktagmax) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "number should be in %d - %d", 1, ktagmax);
		return;
	}

	php_pb_unknown_field_set_number(instance, number TSRMLS_CC);
}
/* }}} */

/* {{{ proto void ProtocolBuffersUnknownField::setType(int $type)
*/
PHP_METHOD(protocolbuffers_unknown_field, setType)
{
	long type = 0;
	zval *instance = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &type) == FAILURE) {
		return;
	}

	php_pb_unknown_field_set_type(instance, type TSRMLS_CC);
}
/* }}} */

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
	for(zend_hash_internal_pointer_reset_ex(field->ht, &pos);
						zend_hash_get_current_data_ex(field->ht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(field->ht, &pos)
		) {
		zval *tmp;
		MAKE_STD_ZVAL(tmp);

		ZVAL_LONG(tmp, (*element)->varint);
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

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	//RETURN_LONG(field->value.fixed32);
}
/* }}} */

/* {{{ proto long ProtocolBuffersUnknownField::getAsFixed64List()
*/
PHP_METHOD(protocolbuffers_unknown_field, getAsFixed64List)
{
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	//RETURN_LONG(field->value.varint);
}
/* }}} */


static zend_function_entry php_protocolbuffers_unknown_field_methods[] = {
	PHP_ME(protocolbuffers_unknown_field, setNumber, arginfo_pb_unknown_field_set_number, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, setType, arginfo_pb_unknown_field_set_type, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getNumber, arginfo_pb_unknown_field_get_number, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getType, arginfo_pb_unknown_field_get_type, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsVarintList, arginfo_pb_unknown_field_get_as_varint, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsLengthDelimitedList, arginfo_pb_unknown_field_get_as_length_delimited, ZEND_ACC_PUBLIC)
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
#endif

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "UnknownField", protocol_buffers_unknown_field_class_entry);
}
