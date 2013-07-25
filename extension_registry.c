#include "php_protocol_buffers.h"
#include "extension_registry.h"

static void php_protocolbuffers_extension_registry_free_storage(php_protocolbuffers_extension_registry *object TSRMLS_DC)
{
	zend_object_std_dtor(&object->zo TSRMLS_CC);
	efree(object);
}

zend_object_value php_protocolbuffers_extension_registry_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	PHP_PROTOCOLBUFFERS_STD_CREATE_OBJECT(php_protocolbuffers_extension_registry);

	return retval;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_extension_registry_add, 0, 0, 4)
	ZEND_ARG_INFO(0, message_class_name)
	ZEND_ARG_INFO(0, extension)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, field_type)
	ZEND_ARG_INFO(0, child_message_class_name)
ZEND_END_ARG_INFO()


/* {{{ proto ProtocolBuffersExtensionRegistry ProtocolBuffersExtensionRegistry::__construct()
*/
PHP_METHOD(protocolbuffers_extension_registry, __construct)
{
}
/* }}} */

static zval *php_protocolbuffers_extension_registry_get_instance(TSRMLS_D)
{
	zval *extension_registry;

	if (PBG(extension_registry) == NULL) {
		MAKE_STD_ZVAL(extension_registry);
		object_init_ex(extension_registry, protocol_buffers_extension_registry_class_entry);
		PBG(extension_registry) = extension_registry;
	}

	return PBG(extension_registry);
}

/* {{{ proto ProtocolBuffersExtensionRegistry ProtocolBuffersExtensionRegistry::getInstance()
*/
PHP_METHOD(protocolbuffers_extension_registry, getInstance)
{
	zval *extension_registry;

	extension_registry = php_protocolbuffers_extension_registry_get_instance(TSRMLS_C);
	RETURN_ZVAL(extension_registry, 1, 0);
}
/* }}} */

/* {{{ proto void ProtocolBuffersExtensionRegistry::add(string $message_class_name, long $extension, string $name, long $field_type[, string $child_message_class_name])
*/
PHP_METHOD(protocolbuffers_extension_registry, add)
{
	zval *message_class_name, child_message_class_name;
	long extension, field_type;
	char *name;
	long name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"zlsl|z", &message_class_name, &extension, &name, &name_len, &field_type, &child_message_class_name) == FAILURE) {
		return;
	}
}
/* }}} */

static zend_function_entry php_protocolbuffers_extension_registry_methods[] = {
	PHP_ME(protocolbuffers_extension_registry, __construct,  NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(protocolbuffers_extension_registry, getInstance,  NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(protocolbuffers_extension_registry, add,  arginfo_pb_extension_registry_add, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void php_pb_extension_registry_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersExtensionRegistry", php_protocolbuffers_extension_registry_methods);
	protocol_buffers_extension_registry_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	protocol_buffers_extension_registry_class_entry->create_object = php_protocolbuffers_extension_registry_new;

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "ExtensionRegistry", protocol_buffers_extension_registry_class_entry);
}
