#include "php_protocol_buffers.h"
#include "descriptor.h"


ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_get_field, 0, 0, 1)
    ZEND_ARG_INFO(0, tag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_get_option, 0, 0, 1)
    ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

/* {{{ proto ProtocolBuffers_FieldDescriptor ProtocolBuffers_Descriptor::getField(int $tag)
*/
PHP_METHOD(protocolbuffers_descriptor, getField)
{
}
/* }}} */

/* {{{ proto mixed ProtocolBuffers_Descriptor::getOption(int $option)
*/
PHP_METHOD(protocolbuffers_descriptor, getOption)
{
}
/* }}} */

static zend_function_entry php_protocolbuffers_descriptor_methods[] = {
    PHP_ME(protocolbuffers_descriptor, getField,  arginfo_pb_descriptor_get_field, ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers_descriptor, getOption, arginfo_pb_descriptor_get_option, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void php_pb_descriptor_class(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ProtocolBuffers_Descriptor", php_protocolbuffers_descriptor_methods);
    protocol_buffers_descriptor_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
}
