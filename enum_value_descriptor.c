#include "protocolbuffers.h"
#include "enum_value_descriptor.h"

static zend_function_entry protocolbuffers_enum_value_descriptor_methods[] = {
	PHP_FE_END
};

void php_protocolbuffers_enum_value_descriptor_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersEnumValueDescriptor", protocolbuffers_enum_value_descriptor_methods);
	php_protocol_buffers_enum_value_descriptor_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "EnumValueDescriptor", php_protocol_buffers_enum_value_descriptor_class_entry);
}
