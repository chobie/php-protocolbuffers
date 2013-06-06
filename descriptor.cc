#include "php_protocol_buffers.h"
#include "descriptor.h"

static void php_protocolbuffers_descriptor_free_storage(php_protocolbuffers_descriptor *object TSRMLS_DC)
{
    if (object->name_len > 0) {
        efree(object->name);
    }

    if (object->container != NULL) {
        int i;

        for (i = 0; i < (object->container)->size; i++) {
            if ((object->container)->scheme[i].name != NULL) {
                efree((object->container)->scheme[i].name);
            }
        }

        if (object->container->scheme != NULL) {
            efree(object->container->scheme);
        }
        efree(object->container);
    }

    zend_object_std_dtor(&object->zo TSRMLS_CC);
    efree(object);
}

zend_object_value php_protocolbuffers_descriptor_new(zend_class_entry *ce TSRMLS_DC)
{
    zend_object_value retval;
    PHP_PROTOCOLBUFFERS_STD_CREATE_OBJECT(php_protocolbuffers_descriptor);

    object->name = NULL;
    object->name_len = 0;
    object->container = (pb_scheme_container*)emalloc(sizeof(pb_scheme_container));

    return retval;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_get_name, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_get_field, 0, 0, 1)
    ZEND_ARG_INFO(0, tag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_get_option, 0, 0, 1)
    ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_dump, 0, 0, 0)
ZEND_END_ARG_INFO()


/* {{{ proto ProtocolBuffers_Descriptor ProtocolBuffers_Descriptor::__construct()
*/
PHP_METHOD(protocolbuffers_descriptor, __construct)
{
    // do not create instance by user. we expect it always created from builder.
}
/* }}} */

/* {{{ proto string ProtocolBuffers_Descriptor::getName()
*/
PHP_METHOD(protocolbuffers_descriptor, getName)
{
    zval *instance = getThis();
    php_protocolbuffers_descriptor *descriptor;

    descriptor = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_descriptor, instance);

    RETURN_STRINGL(descriptor->name, descriptor->name_len, 1);
}
/* }}} */

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

/* {{{ proto void ProtocolBuffers_Descriptor::dump()
*/
PHP_METHOD(protocolbuffers_descriptor, dump)
{
    zval *instance = getThis();
    php_protocolbuffers_descriptor *descriptor;

    descriptor = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_descriptor, instance);

    php_printf("{\n");

    if (descriptor->name_len > 0) {
        php_printf("  \"name\": \"%s\",\n", descriptor->name);
    } else {
        php_printf("  \"name\": \"\",\n");
    }

    php_printf("  \"fields\": {\n");
    php_printf("  }\n");

    php_printf("}\n");
}
/* }}} */

static zend_function_entry php_protocolbuffers_descriptor_methods[] = {
    PHP_ME(protocolbuffers_descriptor, __construct, arginfo_pb_descriptor___construct, ZEND_ACC_PRIVATE)
    PHP_ME(protocolbuffers_descriptor, getName,     arginfo_pb_descriptor_get_name, ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers_descriptor, getField,    arginfo_pb_descriptor_get_field, ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers_descriptor, getOption,   arginfo_pb_descriptor_get_option, ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers_descriptor, dump,        arginfo_pb_descriptor_dump, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void php_pb_descriptor_class(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ProtocolBuffers_Descriptor", php_protocolbuffers_descriptor_methods);
    protocol_buffers_descriptor_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
    protocol_buffers_descriptor_class_entry->create_object = php_protocolbuffers_descriptor_new;
}
