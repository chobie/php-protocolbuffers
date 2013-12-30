#include "protocolbuffers.h"
#include "enum_descriptor.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_enum_descriptor_is_valid, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()


/* {{{ proto void ProtocolBuffersEnumDescriptor::__construct()
*/
PHP_METHOD(protocolbuffers_enum_descriptor, __construct)
{
}
/* }}} */

/* {{{ proto bool ProtocolBuffersEnumDescriptor::isValid(long $value)
*/
PHP_METHOD(protocolbuffers_enum_descriptor, isValid)
{
	zval *values, **entry;
	long value = 0;
	HashPosition pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &value) == FAILURE) {
		return;
	}

	php_protocolbuffers_read_protected_property(getThis(), ZEND_STRS("values"), &values TSRMLS_CC);
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(values), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(values), (void **)&entry, &pos) == SUCCESS) {
		if (Z_LVAL_PP(entry) == value) {
			RETURN_TRUE;
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(values), &pos);
	}

	RETURN_FALSE;
}
/* }}} */


static zend_function_entry protocolbuffers_enum_descriptor_methods[] = {
	PHP_ME(protocolbuffers_enum_descriptor, __construct, NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(protocolbuffers_enum_descriptor, isValid, arginfo_protocolbuffers_enum_descriptor_is_valid, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void php_protocolbuffers_enum_descriptor_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersEnumDescriptor", protocolbuffers_enum_descriptor_methods);
	php_protocol_buffers_enum_descriptor_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "EnumDescriptor", php_protocol_buffers_enum_descriptor_class_entry);
}
