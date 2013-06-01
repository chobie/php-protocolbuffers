#include "php_protocol_buffers.h"
#include "message_options.h"


static zend_function_entry php_protocolbuffers_message_options_methods[] = {
    {NULL, NULL, NULL}
};

void php_pb_message_options_class(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ProtocolBuffers_MessageOptions", php_protocolbuffers_message_options_methods);
    protocol_buffers_message_options_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
}
