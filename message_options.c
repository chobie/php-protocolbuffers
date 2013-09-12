#include "php_protocol_buffers.h"
#include "message_options.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_message_options_get_options, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

/* {{{ proto ProtocolBuffers_MessageOptions::getExtension(string $name)
*/
PHP_METHOD(protocolbuffers_message_options, getExtension)
{
	zval **result, *options;
	char *name;
	long name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"s", &name, &name_len) == FAILURE) {
		return;
	}

	options = zend_read_property(protocol_buffers_descriptor_builder_class_entry, getThis(), "extensions", sizeof("extensions")-1, 1 TSRMLS_CC);

	if (zend_hash_find(Z_ARRVAL_P(options), name, name_len, (void **)&result) != SUCCESS) {
		if (strcmp(name, "php") == 0) {
			zval *obj;

			MAKE_STD_ZVAL(obj);
			object_init_ex(obj, protocol_buffers_php_message_options_class_entry);

			zend_hash_update(Z_ARRVAL_P(options), name, name_len+1, (void **)&obj, sizeof(obj), NULL);
			result = &obj;
		} else {
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "%s extension does not support. now only supports php extension", name);
			return;
		}
	}

	RETURN_ZVAL(*result, 1, 0);
}
/* }}} */


int php_protocolbuffers_message_options_init_properties(zval *object TSRMLS_DC)
{
	HashTable *properties = NULL;
	zval *tmp = NULL;

	ALLOC_HASHTABLE(properties);
	zend_hash_init(properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	MAKE_STD_ZVAL(tmp);
	array_init(tmp);
	zend_hash_update(properties, "extensions", sizeof("extensions"), (void **)&tmp, sizeof(zval), NULL);

	zend_merge_properties(object, properties, 1 TSRMLS_CC);

	return 0;
}

static zend_function_entry php_protocolbuffers_message_options_methods[] = {
	PHP_ME(protocolbuffers_message_options, getExtension, arginfo_pb_message_options_get_options, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void php_pb_message_options_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersMessageOptions", php_protocolbuffers_message_options_methods);
	protocol_buffers_message_options_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	zend_declare_property_null(protocol_buffers_message_options_class_entry, "extensions", sizeof("extensions")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "MessageOptions", protocol_buffers_message_options_class_entry);
}
