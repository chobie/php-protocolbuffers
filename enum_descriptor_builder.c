#include "protocolbuffers.h"
#include "enum_descriptor_builder.h"

static zend_function_entry protocolbuffers_enum_descriptor_builder_methods[] = {
	PHP_FE_END
};

void php_protocolbuffers_enum_descriptor_builder_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersEnumDescriptorBuilder", protocolbuffers_enum_descriptor_builder_methods);
	php_protocol_buffers_enum_descriptor_builder_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "EnumDescriptorBuilder", php_protocol_buffers_enum_descriptor_builder_class_entry);
}
