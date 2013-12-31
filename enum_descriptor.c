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

static zend_function_entry protocolbuffers_enum_descriptor_methods[] = {
	PHP_ME(protocolbuffers_enum_descriptor, __construct, NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_FE_END
};

void php_protocolbuffers_enum_descriptor_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersEnumDescriptor", protocolbuffers_enum_descriptor_methods);
	php_protocol_buffers_enum_descriptor_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "EnumDescriptor", php_protocol_buffers_enum_descriptor_class_entry);
}
