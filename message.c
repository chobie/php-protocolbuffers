#include "php_protocol_buffers.h"
#include "message.h"


static zend_function_entry php_protocolbuffers_message_methods[] = {
	{NULL, NULL, NULL}
};

void php_pb_message_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffers_Message", php_protocolbuffers_message_methods);
	protocol_buffers_message_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
}
