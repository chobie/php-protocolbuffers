#include "php_protocol_buffers.h"
#include "unknown_field_set.h"


static zend_function_entry php_protocolbuffers_unknown_field_set_methods[] = {
	{NULL, NULL, NULL}
};

void php_pb_unknown_field_set_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersUnknownFieldSet", php_protocolbuffers_unknown_field_set_methods);
	protocol_buffers_unknown_field_set_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
}
