#include "php_protocol_buffers.h"
#include "unknown_field_set.h"

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
}
/* }}} */

/* {{{ proto ProtocolBuffersUnknownField ProtocolBuffersUnknownFieldSet::getField(int $index)
*/
PHP_METHOD(protocolbuffers_unknown_field_set, getField)
{
}
/* }}} */

/* {{{ proto void ProtocolBuffersUnknownFieldSet::addField(ProtocolBuffersUnknownField $field)
*/
PHP_METHOD(protocolbuffers_unknown_field_set, addField)
{
}
/* }}} */


static zend_function_entry php_protocolbuffers_unknown_field_set_methods[] = {
	PHP_ME(protocolbuffers_unknown_field_set, count, arginfo_pb_unknown_field_set_count, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field_set, getField, arginfo_pb_unknown_field_get_field, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field_set, addField, arginfo_pb_unknown_field_add_field, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void php_pb_unknown_field_set_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersUnknownFieldSet", php_protocolbuffers_unknown_field_set_methods);
	protocol_buffers_unknown_field_set_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
}
