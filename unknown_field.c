#include "php_protocol_buffers.h"
#include "unknown_field.h"


static zend_function_entry php_protocolbuffers_unknown_field_methods[] = {
	{NULL, NULL, NULL}
};

void php_pb_unknown_field_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersUnknownField", php_protocolbuffers_unknown_field_methods);
	protocol_buffers_unknown_field_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
}
