#include "php_protocol_buffers.h"
#include "extension_registry.h"

static void php_protocolbuffers_extension_registry_free_storage(php_protocolbuffers_extension_registry *object TSRMLS_DC)
{
	if (object->registry != NULL) {
		HashPosition pos;
		int n = 0;
		zval **element;

		for(n = 0, zend_hash_internal_pointer_reset_ex(object->registry, &pos);
			zend_hash_get_current_data_ex(object->registry, (void **)&element, &pos) == SUCCESS;
			zend_hash_move_forward_ex(object->registry, &pos), n++
		) {
            zval_ptr_dtor(element);
		}

		zend_hash_destroy(object->registry);
		efree(object->registry);
		object->registry = NULL;
	}

	zend_object_std_dtor(&object->zo TSRMLS_CC);
	efree(object);
}

zend_object_value php_protocolbuffers_extension_registry_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	PHP_PROTOCOLBUFFERS_STD_CREATE_OBJECT(php_protocolbuffers_extension_registry);

	ALLOC_HASHTABLE(object->registry);
	zend_hash_init(object->registry, 0, NULL, NULL, 0);

	return retval;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_extension_registry_add, 0, 0, 3)
	ZEND_ARG_INFO(0, message_class_name)
	ZEND_ARG_INFO(0, extension)
	ZEND_ARG_INFO(0, descriptor)
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

/* {{{ proto void ProtocolBuffersExtensionRegistry::add(string $message_class_name, long $extension, ProtocolBuffersFieldDescriptor $descriptor)
*/
PHP_METHOD(protocolbuffers_extension_registry, add)
{
	zval *instance = getThis();
	char *message_class_name;
	long message_class_name_len;
	zval *descriptor, **bucket;
	zend_class_entry **ce;
	long extension;
	php_protocolbuffers_extension_registry *registry;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"slO", &message_class_name, &message_class_name_len, &extension, &descriptor, protocol_buffers_field_descriptor_class_entry) == FAILURE) {
		return;
	}

	if (zend_lookup_class((char*)message_class_name, message_class_name_len, &ce TSRMLS_CC) == FAILURE) {
		// TODO: check the class has getDescriptor method.
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "%s class does not find", message_class_name);
		return;
	}

	registry = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_extension_registry, instance);
	if (!zend_hash_exists(registry->registry, message_class_name, message_class_name_len)) {
		zval *p = NULL;

		MAKE_STD_ZVAL(p);
		array_init(p);

		Z_ADDREF_P(p);
		zend_hash_update(registry->registry, message_class_name, message_class_name_len, (void **)&p, sizeof(zval*), NULL);
		zval_ptr_dtor(&p);
		p = NULL;
	}

	if (zend_hash_find(registry->registry, message_class_name, message_class_name_len, (void **)&bucket) == SUCCESS) {
		if (zend_hash_index_exists(Z_ARRVAL_PP(bucket), extension)) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "can't override specified extension number: already exists");
			return;
		}

		zend_hash_index_update(Z_ARRVAL_PP(bucket), extension, (void **)&descriptor, sizeof(zval*), NULL);
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
