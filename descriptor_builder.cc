#include "php_protocol_buffers.h"
#include "descriptor_builder.h"

static void php_protocolbuffers_descriptor_builder_free_storage(zend_object *object TSRMLS_DC)
{
    zend_object_std_dtor(object TSRMLS_CC);
    efree(object);
}

zend_object_value php_protocolbuffers_descriptor_builder_new(zend_class_entry *ce TSRMLS_DC)
{
    zend_object_value retval;
    zend_object *object;

    object = (zend_object*)ecalloc(1, sizeof(*object));
    zend_object_std_init(object, ce TSRMLS_CC);

#if ZEND_MODULE_API_NO >= 20100525
    object_properties_init(object, ce);
#else
    {
        zval *tmp;
        zend_hash_copy(object->properties, &ce->default_properties, (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));
    }
#endif
    retval.handle = zend_objects_store_put(object,
        (zend_objects_store_dtor_t)zend_objects_destroy_object,
        (zend_objects_free_object_storage_t) php_protocolbuffers_descriptor_builder_free_storage,
    NULL TSRMLS_CC);
    retval.handlers = zend_get_std_object_handlers();

    return retval;
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_add_field, 0, 0, 2)
    ZEND_ARG_INFO(0, index)
    ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_get_name, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_set_name, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_build, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_descriptor_builder_set_extension, 0, 0, 2)
    ZEND_ARG_INFO(0, index)
    ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

/* {{{ proto void ProtocolBuffers_DescriptorBuilder::addField(int $index, ProtocolBuffers_DescriptorField $field)
*/
PHP_METHOD(protocolbuffers_descriptor_builder, addField)
{
    zval *instance = getThis();
    zval *field = NULL;
    zval **fields = NULL;
    long index;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "lO", &index, &field, protocol_buffers_field_descriptor_class_entry) == FAILURE) {
        return;
    }

    if (index < 1 || index > ktagmax) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "index shoud be in %d - %d", 1, ktagmax);
        return;
    }

    if (zend_hash_find(Z_OBJPROP_P(instance), "fields", sizeof("fields"), (void **)&fields) == SUCCESS) {
        zend_hash_index_update(Z_ARRVAL_PP(fields), index, (void**)&field, sizeof(zval *), NULL);
    } else {
        MAKE_STD_ZVAL(*fields);
        array_init(*fields);

        zend_hash_index_update(Z_ARRVAL_PP(fields), index, (void**)&field, sizeof(zval *), NULL);
        zend_hash_update(Z_OBJPROP_P(instance), "fields", sizeof("fields"), (void **)&fields, sizeof(zval*), NULL);
    }
}
/* }}} */

/* {{{ proto void ProtocolBuffers_DescriptorBuilder::setName(string $name)
*/
PHP_METHOD(protocolbuffers_descriptor_builder, setName)
{
    zval *instance = getThis();
    char *name;
    long name_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "s", &name, &name_len) == FAILURE) {
        return;
    }

    if (name_len > 0) {
        add_property_string_ex(instance, "name", sizeof("name"), name, 1 TSRMLS_CC);
    }
}
/* }}} */

/* {{{ proto string ProtocolBuffers_DescriptorBuilder::getName()
*/
PHP_METHOD(protocolbuffers_descriptor_builder, getName)
{
    zval *result, *name;
    php_protocolbuffers_descriptor *descriptor;

    name = zend_read_property(protocol_buffers_descriptor_builder_class_entry, getThis(), "name", sizeof("name")-1, 0 TSRMLS_CC);
    RETURN_STRINGL(Z_STRVAL_P(name), Z_STRLEN_P(name), 1);
}
/* }}} */


/* {{{ proto ProtocolBuffers_Descriptor ProtocolBuffers_DescriptorBuilder::build()
*/
PHP_METHOD(protocolbuffers_descriptor_builder, build)
{
    zval *result, *name;
    php_protocolbuffers_descriptor *descriptor;

    MAKE_STD_ZVAL(result);
    object_init_ex(result, protocol_buffers_descriptor_class_entry);

    descriptor = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_descriptor, result);

    name = zend_read_property(protocol_buffers_descriptor_builder_class_entry, getThis(), "name", sizeof("name")-1, 0 TSRMLS_CC);
    if (name) {
        descriptor->name_len = Z_STRLEN_P(name);
        descriptor->name = (char*)emalloc(descriptor->name_len+1);
        memset(descriptor->name, '\0', descriptor->name_len+1);
        memcpy(descriptor->name, Z_STRVAL_P(name), descriptor->name_len);
    }

    RETURN_ZVAL(result, 0, 1);
}
/* }}} */


/* {{{ proto void ProtocolBuffers_DescriptorBuilder::setExtension()
*/
PHP_METHOD(protocolbuffers_descriptor_builder, setExtension)
{
    zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "ProtocolBuffers_DescriptorBuilder::setExtension does not implement yet");
}
/* }}} */


static zend_function_entry php_protocolbuffers_descriptor_builder_methods[] = {
    PHP_ME(protocolbuffers_descriptor_builder, addField,  arginfo_pb_descriptor_builder_add_field, ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers_descriptor_builder, getName,   arginfo_pb_descriptor_builder_get_name, ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers_descriptor_builder, setName,   arginfo_pb_descriptor_builder_set_name, ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers_descriptor_builder, build,     arginfo_pb_descriptor_builder_build, ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers_descriptor_builder, setExtension, arginfo_pb_descriptor_builder_set_extension, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void php_pb_descriptor_builder_class(TSRMLS_D)
{
    zend_class_entry ce;
    zval *fields;

    INIT_CLASS_ENTRY(ce, "ProtocolBuffers_DescriptorBuilder", php_protocolbuffers_descriptor_builder_methods);
    protocol_buffers_descriptor_builder_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
    protocol_buffers_descriptor_builder_class_entry->create_object = php_protocolbuffers_descriptor_builder_new;

    MAKE_STD_ZVAL(fields);
    array_init(fields);

    zend_declare_property_null(protocol_buffers_descriptor_builder_class_entry, "name", sizeof("name")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(protocol_buffers_descriptor_builder_class_entry, "fields", sizeof("fields")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
}
