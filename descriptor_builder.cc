#include "php_protocol_buffers.h"
#include "descriptor_builder.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_add_field, 0, 0, 2)
    ZEND_ARG_INFO(0, index)
    ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_set_name, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_build, 0, 0, 0)
ZEND_END_ARG_INFO()

/* {{{ proto void ProtocolBuffers_DescriptorBuilder::addField(int $index, ProtocolBuffers_DescriptorField $field)
*/
PHP_METHOD(protocolbuffers_descriptor_builder, addField)
{
}
/* }}} */

/* {{{ proto void ProtocolBuffers_DescriptorBuilder::setName(string $name)
*/
PHP_METHOD(protocolbuffers_descriptor_builder, setName)
{
}
/* }}} */


/* {{{ proto ProtocolBuffers_Descriptor ProtocolBuffers_DescriptorBuilder::build()
*/
PHP_METHOD(protocolbuffers_descriptor_builder, build)
{
    zval *result;


    MAKE_STD_ZVAL(result);
    object_init_ex(result, protocol_buffers_descriptor_class_entry);

    RETURN_ZVAL(result, 0, 1);
}
/* }}} */


static zend_function_entry php_protocolbuffers_descriptor_builder_methods[] = {
    PHP_ME(protocolbuffers_descriptor_builder, addField,  arginfo_pb_descriptor_builder_add_field, ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers_descriptor_builder, setName,   arginfo_pb_descriptor_builder_set_name, ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers_descriptor_builder, build,     arginfo_pb_descriptor_builder_build, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void php_pb_descriptor_builder_class(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ProtocolBuffers_DescriptorBuilder", php_protocolbuffers_descriptor_builder_methods);
    protocol_buffers_descriptor_builder_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
}
