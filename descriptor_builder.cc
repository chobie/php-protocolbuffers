#include "php_protocol_buffers.h"
#include "descriptor_builder.h"



ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_add_field, 0, 0, 1)
    ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

/* {{{ proto void ProtocolBuffers_DescriptorBuilder::addField(ProtocolBuffers_DescriptorField $field)
*/
PHP_METHOD(protocolbuffers_descriptor_builder, addField)
{
}
/* }}} */

static zend_function_entry php_protocolbuffers_descriptor_builder_methods[] = {
    PHP_ME(protocolbuffers_descriptor_builder, addField,  arginfo_pb_descriptor_builder_add_field, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void php_pb_descriptor_builder_class(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ProtocolBuffers_DescriptorBuilder", php_protocolbuffers_descriptor_builder_methods);
    protocol_buffers_descriptor_builder_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
}
