#include "php_protocol_buffers.h"
#include "extension_registry.h"


static zend_function_entry php_protocolbuffers_extension_registry_methods[] = {
	{NULL, NULL, NULL}
};

void php_pb_extension_registry_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersExtensionRegistry", php_protocolbuffers_extension_registry_methods);
	protocol_buffers_extension_registry_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "ExtensionRegistry", protocol_buffers_extension_registry_class_entry);
}
