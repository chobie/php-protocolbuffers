#include "php_protocol_buffers.h"
#include "field_descriptor.h"

static int php_protocolbuffers_field_descriptor_process_params(zval **zv TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key)
{
    if (hash_key->nKeyLength == 0) {
        return 0;
    } else {
        const char *key = hash_key->arKey;
        size_t key_length = hash_key->nKeyLength;
        zval *value = NULL, **instance = NULL;
        char *name;
        int name_length;

        instance = va_arg(args, zval**);

#define PHP_PROTOCOLBUFFERS_PROCESS_BOOL \
{\
    zend_mangle_property_name(&name, &name_length, (char*)"*", 1, (char*)key, key_length, 0);\
    \
    MAKE_STD_ZVAL(value);\
    if (Z_TYPE_PP(zv) != IS_BOOL) {\
        convert_to_boolean(*zv);\
    }\
    \
    ZVAL_BOOL(value, Z_BVAL_PP(zv));\
    zend_hash_update(Z_OBJPROP_PP(instance), name, name_length, (void **)&value, sizeof(zval*), NULL);\
    efree(name);\
}

        if (strcmp(key, "type") == 0) {
            if (Z_TYPE_PP(zv) != IS_LONG) {
                convert_to_long(*zv);
            }

            if (Z_LVAL_PP(zv) > MAX_FIELD_TYPE) {
                return 1;
            }
            if (Z_LVAL_PP(zv) < 1) {
                return 1;
            }
            if (Z_LVAL_PP(zv) == TYPE_GROUP) {
                return 1;
            }

            zend_mangle_property_name(&name, &name_length, (char*)"*", 1, (char*)key, key_length, 0);
            MAKE_STD_ZVAL(value);
            ZVAL_LONG(value, Z_LVAL_PP(zv));
            zend_hash_update(Z_OBJPROP_PP(instance), name, name_length, (void **)&value, sizeof(zval*), NULL);
            efree(name);

        } else if (strcmp(key, "required") == 0) {
            PHP_PROTOCOLBUFFERS_PROCESS_BOOL;
        } else if (strcmp(key, "optional") == 0) {
            PHP_PROTOCOLBUFFERS_PROCESS_BOOL;
        } else if (strcmp(key, "repeated") == 0) {
            PHP_PROTOCOLBUFFERS_PROCESS_BOOL;
        } else if (strcmp(key, "packable") == 0) {
            PHP_PROTOCOLBUFFERS_PROCESS_BOOL;
        } else if (strcmp(key, "default") == 0) {
            zend_mangle_property_name(&name, &name_length, (char*)"*", 1, (char*)key, key_length, 0);

            MAKE_STD_ZVAL(value);
            ZVAL_ZVAL(value, *zv, 1, 0);
            zend_hash_update(Z_OBJPROP_PP(instance), name, name_length, (void **)&value, sizeof(zval*), NULL);
            efree(name);
        } else {
        }
    }

#undef PHP_PROTOCOLBUFFERS_PROCESS_BOOL

    return 0;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_field_descriptor___construct, 0, 0, 1)
    ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_field_descriptor_get_type, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_field_descriptor_set_type, 0, 0, 1)
    ZEND_ARG_INFO(0, wiretype)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_field_descriptor_get_default, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_field_descriptor_set_default, 0, 0, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()


/* {{{ proto ProtocolBuffers_FieldDescriptor ProtocolBuffers_FieldDescriptor::__construct(array $params)
*/
PHP_METHOD(protocolbuffers_field_descriptor, __construct)
{
    zval *instance = getThis();
    zval *params = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "|a", &params) == FAILURE) {
        return;
    }

    if (params) {
        zend_hash_apply_with_arguments(Z_ARRVAL_P(params) TSRMLS_CC, (apply_func_args_t) php_protocolbuffers_field_descriptor_process_params, 1, &instance);
    }
}
/* }}} */


/* {{{ proto long ProtocolBuffers_FieldDescriptor::getType()
*/
PHP_METHOD(protocolbuffers_field_descriptor, getType)
{
    zval *instance = getThis();
    zval **result = NULL;
    char *name = NULL;
    int name_len = 0;

    zend_mangle_property_name(&name, &name_len, "*", 1, "type", sizeof("type"), 0);
    if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&result) == SUCCESS) {
        RETVAL_LONG(Z_LVAL_PP(result))
    } else {
        RETVAL_LONG(-1);
    }
    efree(name);
}
/* }}} */

/* {{{ proto void ProtocolBuffers_FieldDescriptor::setType(long $wiretype)
*/
PHP_METHOD(protocolbuffers_field_descriptor, setType)
{

    zval *instance = getThis();
    zval *value = NULL;
    long wiretype;
    char *name;
    int name_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "l", &wiretype) == FAILURE) {
        return;
    }

    zend_mangle_property_name(&name, &name_len, "*", 1, "type", sizeof("type"), 0);

    MAKE_STD_ZVAL(value);
    ZVAL_LONG(value, wiretype);
    zend_hash_update(Z_OBJPROP_P(instance), name, name_len, (void **)&value, sizeof(zval*), NULL);
    efree(name);
}
/* }}} */

/* {{{ proto mixed ProtocolBuffers_FieldDescriptor::getDefault()
*/
PHP_METHOD(protocolbuffers_field_descriptor, getDefault)
{
    zval *instance = getThis();
    zval **result = NULL;
    char *name = NULL;
    int name_len = 0;

    zend_mangle_property_name(&name, &name_len, "*", 1, "default", sizeof("default"), 0);
    if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&result) == SUCCESS) {
        RETVAL_ZVAL(*result, 1, 0);
    } else {
        RETVAL_NULL();
    }
    efree(name);
}
/* }}} */

/* {{{ proto void ProtocolBuffers_FieldDescriptor::setDefault(mixed $value)
*/
PHP_METHOD(protocolbuffers_field_descriptor, setDefault)
{
    zval *instance = getThis();
    zval *zv = NULL, *value = NULL;
    char *name;
    int name_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "z", &value) == FAILURE) {
        return;
    }

    zend_mangle_property_name(&name, &name_len, "*", 1, "default", sizeof("default"), 0);

    MAKE_STD_ZVAL(zv);
    ZVAL_ZVAL(zv, value, 1, 0);
    zend_hash_update(Z_OBJPROP_P(instance), name, name_len, (void **)&zv, sizeof(zval*), NULL);
    efree(name);
}
/* }}} */

static zend_function_entry php_protocolbuffers_field_descriptor_methods[] = {
    PHP_ME(protocolbuffers_field_descriptor, __construct,  arginfo_pb_field_descriptor___construct, ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers_field_descriptor, getType,  arginfo_pb_field_descriptor_get_type, ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers_field_descriptor, setType,  arginfo_pb_field_descriptor_set_type, ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers_field_descriptor, getDefault, arginfo_pb_field_descriptor_get_default, ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers_field_descriptor, setDefault, arginfo_pb_field_descriptor_set_default, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void php_pb_filed_descriptor_class(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ProtocolBuffers_FieldDescriptor", php_protocolbuffers_field_descriptor_methods);
    protocol_buffers_field_descriptor_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
}
