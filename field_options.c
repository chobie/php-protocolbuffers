#include "php_protocol_buffers.h"
#include "field_options.h"

static zend_function_entry php_protocolbuffers_field_options_methods[] = {
	{NULL, NULL, NULL}
};

void php_pb_filed_options_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffers_FieldOptions", php_protocolbuffers_field_options_methods);
	protocol_buffers_field_options_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
}
