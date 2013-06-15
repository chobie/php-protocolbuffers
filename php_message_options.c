#include "php_protocol_buffers.h"
#include "php_message_options.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_php_message_options_set_use_single_property, 0, 0, 1)
	ZEND_ARG_INFO(0, use)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_php_message_options_set_single_property_name, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_php_message_options_set_process_unkown_fields, 0, 0, 1)
	ZEND_ARG_INFO(0, use)
ZEND_END_ARG_INFO()

/* {{{ proto void ProtocolBuffersPHPMessageOptions::setUseSingleProperty(boolean $use)
*/
PHP_METHOD(protocolbuffers_php_message_options, setUseSingleProperty)
{

	zval *instance = getThis();
	zval **target;
	long value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &value) == FAILURE) {
		return;
	}

	if (zend_hash_find(Z_OBJPROP_P(instance), "use_single_property", sizeof("use_single_property"), (void **)&target) == SUCCESS) {
		ZVAL_BOOL(*target, value);
	}
}
/* }}} */

/* {{{ proto void ProtocolBuffersPHPMessageOptions::setSinglePropertyName(string $name)
*/
PHP_METHOD(protocolbuffers_php_message_options, setSinglePropertyName)
{
	zval *instance = getThis();
	zval **target;
	char *value;
	long value_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"s", &value, &value_len) == FAILURE) {
		return;
	}


	if (zend_hash_find(Z_OBJPROP_P(instance), "single_property_name", sizeof("single_property_name"), (void **)&target) == SUCCESS) {
		if (Z_STRVAL_PP(target) != NULL) {
			zval_ptr_dtor(target);
			MAKE_STD_ZVAL(*target);
		}

		ZVAL_STRING(*target, value, 1);
	}
}
/* }}} */

/* {{{ proto void ProtocolBuffersPHPMessageOptions::setProcessUnknownFields(boolean $use)
*/
PHP_METHOD(protocolbuffers_php_message_options, setProcessUnknownFields)
{
	zval *instance = getThis();
	zval **target;
	long value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &value) == FAILURE) {
		return;
	}

	if (zend_hash_find(Z_OBJPROP_P(instance), "process_unknown_fields", sizeof("process_unknown_fields"), (void **)&target) == SUCCESS) {
		ZVAL_BOOL(*target, value);
	}
}
/* }}} */


static zend_function_entry php_protocolbuffers_php_message_options_methods[] = {
	PHP_ME(protocolbuffers_php_message_options, setUseSingleProperty, arginfo_pb_php_message_options_set_use_single_property, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_php_message_options, setSinglePropertyName, arginfo_pb_php_message_options_set_single_property_name, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_php_message_options, setProcessUnknownFields, arginfo_pb_php_message_options_set_process_unkown_fields, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void php_pb_php_message_options_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersPHPMessageOptions", php_protocolbuffers_php_message_options_methods);
	protocol_buffers_php_message_options_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_bool(protocol_buffers_php_message_options_class_entry, "use_single_property", sizeof("use_single_property")-1, 0, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(protocol_buffers_php_message_options_class_entry, "single_property_name", sizeof("single_property_name")-1, "_properties", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_bool(protocol_buffers_php_message_options_class_entry, "process_unknown_fields", sizeof("process_unknown_fields")-1, 0, ZEND_ACC_PUBLIC TSRMLS_CC);

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "PHPMessageOptions", protocol_buffers_php_message_options_class_entry);
}
