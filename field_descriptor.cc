#include "php_protocol_buffers.h"
#include "field_descriptor.h"

static zend_function_entry php_protocolbuffers_field_descriptor_methods[] = {
    {NULL, NULL, NULL}
};

void php_pb_filed_descriptor_class(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ProtocolBuffers_FieldDescriptor", php_protocolbuffers_field_descriptor_methods);
    protocol_buffers_field_descriptor_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
}
