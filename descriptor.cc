#include "php_protocol_buffers.h"
#include "descriptor.h"

void php_pb_descriptor_class(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ProtocolBuffers_Descriptor", 0);
    protocol_buffers_descriptor_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
}
