#include "php_protocol_buffers.h"
#include "unknown_field.h"

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


/* {{{ proto void ProtocolBuffersUnknownField::setNumber()
*/
PHP_METHOD(protocolbuffers_unknown_field, setNumber)
{
}
/* }}} */

/* {{{ proto void ProtocolBuffersUnknownField::setType()
*/
PHP_METHOD(protocolbuffers_unknown_field, setType)
{
}
/* }}} */

/* {{{ proto void ProtocolBuffersUnknownField::getNumber()
*/
PHP_METHOD(protocolbuffers_unknown_field, getNumber)
{
}
/* }}} */

/* {{{ proto void ProtocolBuffersUnknownField::getType()
*/
PHP_METHOD(protocolbuffers_unknown_field, getType)
{
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
}
