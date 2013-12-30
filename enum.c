#include "protocolbuffers.h"
#include "enum.h"

static zend_function_entry protocolbuffers_enum_methods[] = {
	PHP_FE_END
};

void php_protocolbuffers_enum_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersEnum", protocolbuffers_enum_methods);
	php_protocol_buffers_enum_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "Enum", php_protocol_buffers_enum_class_entry);
}
