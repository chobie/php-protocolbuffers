#include "php_protocol_buffers.h"
#include "unknown_field.h"

static void php_protocolbuffers_unknown_field_free_storage(php_protocolbuffers_unknown_field *object TSRMLS_DC)
{
	zend_object_std_dtor(&object->zo TSRMLS_CC);
	efree(object);
}

zend_object_value php_protocolbuffers_unknown_field_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	PHP_PROTOCOLBUFFERS_STD_CREATE_OBJECT(php_protocolbuffers_unknown_field);

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


/* {{{ proto void ProtocolBuffersUnknownField::setNumber(int $number)
*/
PHP_METHOD(protocolbuffers_unknown_field, setNumber)
{
	long number = 0;
	char *name;
	int name_len;
	zval **result = NULL;
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &number) == FAILURE) {
		return;
	}

	if (number < 1 || number > ktagmax) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "number should be in %d - %d", 1, ktagmax);
		return;
	}


	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"number", sizeof("number"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&result) == SUCCESS) {
		ZVAL_LONG(*result, number);
		field->number = number;
	}
	efree(name);
}
/* }}} */

/* {{{ proto void ProtocolBuffersUnknownField::setType(int $type)
*/
PHP_METHOD(protocolbuffers_unknown_field, setType)
{
	long type = 0;
	char *name;
	int name_len;
	zval **result = NULL;
	zval *instance = getThis();
	php_protocolbuffers_unknown_field *field = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &type) == FAILURE) {
		return;
	}

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"type", sizeof("type"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&result) == SUCCESS) {
		ZVAL_LONG(*result, type);
		field->type = type;
	}
	efree(name);
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


static zend_function_entry php_protocolbuffers_unknown_field_methods[] = {
	PHP_ME(protocolbuffers_unknown_field, setNumber, arginfo_pb_unknown_field_set_number, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, setType, arginfo_pb_unknown_field_set_type, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getNumber, arginfo_pb_unknown_field_get_number, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getType, arginfo_pb_unknown_field_get_type, ZEND_ACC_PUBLIC)
/*
	PHP_ME(protocolbuffers_unknown_field, getAsVarint, arginfo_pb_unknown_field_get_as_varint, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsFixed32, arginfo_pb_unknown_field_get_as_fixed32, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsFixed64, arginfo_pb_unknown_field_get_as_fixed64, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getAsLengthDelimited, arginfo_pb_unknown_field_get_as_fixed64, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field, getLengthDelimitedSize, arginfo_pb_unknown_field_get_length_delimited_size, ZEND_ACC_PUBLIC)
*/
	{NULL, NULL, NULL}
};

void php_pb_unknown_field_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersUnknownField", php_protocolbuffers_unknown_field_methods);
	protocol_buffers_unknown_field_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	protocol_buffers_unknown_field_class_entry->create_object = php_protocolbuffers_unknown_field_new;

	/* for var_dump. */
	zend_declare_property_long(protocol_buffers_unknown_field_class_entry, "number", sizeof("number")-1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(protocol_buffers_unknown_field_class_entry, "type", sizeof("type")-1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);
}
