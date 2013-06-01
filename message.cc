#include "php_protocol_buffers.h"
#include "message.h"

void php_pb_message_class(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ProtocolBuffers_Message", 0);
    protocol_buffers_message_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
}
