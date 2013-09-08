#include "php_protocol_buffers.h"
#include "extension_registry.h"
#include "field_descriptor.h"
#include "descriptor_builder.h"

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

zval *php_protocolbuffers_extension_registry_get_instance(TSRMLS_D)
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

static int sort_cb(const void *a, const void *b)
{
    return ((pb_scheme*)a)->tag - ((pb_scheme*)b)->tag;
}


/* {{{ proto void ProtocolBuffersExtensionRegistry::add(string $message_class_name, long $extension, ProtocolBuffersFieldDescriptor $descriptor)
*/
PHP_METHOD(protocolbuffers_extension_registry, add)
{
	zval *instance = getThis();
	char *message_class_name;
	long message_class_name_len;
	zval *descriptor, **bucket;
	zend_class_entry **ce;
	pb_scheme_container *container = NULL;
	long extension;
	php_protocolbuffers_extension_registry *registry;
	HashTable *proto = NULL;

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
		zval *p, *p2, *p3, *p4 = NULL;

		MAKE_STD_ZVAL(p);
		array_init(p);

		MAKE_STD_ZVAL(p2);
		array_init(p2);

		MAKE_STD_ZVAL(p3);
		array_init(p3);

		MAKE_STD_ZVAL(p4);
		array_init(p4);


		zend_hash_update(Z_ARRVAL_P(p), "map", sizeof("map"), (void **)&p2, sizeof(zval*), NULL);
		zend_hash_update(Z_ARRVAL_P(p), "index", sizeof("index"), (void **)&p3, sizeof(zval*), NULL);
		zend_hash_update(Z_ARRVAL_P(p), "emap", sizeof("emap"), (void **)&p4, sizeof(zval*), NULL);

		Z_ADDREF_P(p);
		zend_hash_update(registry->registry, message_class_name, message_class_name_len, (void **)&p, sizeof(zval*), NULL);
		zval_ptr_dtor(&p);
		p = NULL;
	}

	if (zend_hash_find(registry->registry, message_class_name, message_class_name_len, (void **)&bucket) == SUCCESS) {
		zval **bucket2;
		char *name = NULL;
		int len = 0;

		if (zend_hash_find(Z_ARRVAL_PP(bucket), "index", sizeof("index"), (void **)&bucket2) == SUCCESS) {
			if (zend_hash_index_exists(Z_ARRVAL_PP(bucket), extension)) {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "can't override specified extension number: already exists");
				return;
			}

			Z_ADDREF_P(descriptor);
			zend_hash_index_update(Z_ARRVAL_PP(bucket2), extension, (void **)&descriptor, sizeof(zval*), NULL);
		}

		if (zend_hash_find(Z_ARRVAL_PP(bucket), "map", sizeof("map"), (void **)&bucket2) == SUCCESS) {
			php_pb_field_descriptor_get_name(descriptor, &name, &len TSRMLS_CC);

			if (zend_hash_exists(Z_ARRVAL_PP(bucket), name, len)) {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "can't override specified extension number: already exists");
				return;
			}

			Z_ADDREF_P(descriptor);
			zend_hash_update(Z_ARRVAL_PP(bucket2), name, len, (void **)&descriptor, sizeof(zval*), NULL);

			if (zend_hash_find(Z_ARRVAL_PP(bucket), "emap", sizeof("emap"), (void **)&bucket2) == SUCCESS) {
				zval *nm;

				MAKE_STD_ZVAL(nm);
				ZVAL_LONG(nm, extension);

				zend_hash_update(Z_ARRVAL_PP(bucket2), name, len, (void **)&nm, sizeof(zval*), NULL);
			}
		}

		zend_declare_property_null(*ce, name, len-1, ZEND_ACC_PROTECTED TSRMLS_CC);

		{
			// TODO: Refactor this block
			zval *tmp;

			pb_get_scheme_container((*ce)->name, (*ce)->name_length, &container, proto TSRMLS_CC);
			container->scheme = (pb_scheme*)erealloc(container->scheme, sizeof(pb_scheme) * (container->size + 1));

			container->scheme[container->size].is_extension = 1;
			container->scheme[container->size].tag = extension;

			php_pb_field_descriptor_get_property(Z_OBJPROP_P(descriptor), "type", sizeof("type"), &tmp TSRMLS_CC);
			if (Z_TYPE_P(tmp) == IS_LONG) {
				container->scheme[container->size].type = Z_LVAL_P(tmp);
			}

			php_pb_field_descriptor_get_property(Z_OBJPROP_P(descriptor), "name", sizeof("name"), &tmp TSRMLS_CC);
			if (Z_TYPE_P(tmp) == IS_STRING) {
				char *mangle;
				int mangle_len;
				size_t tsize;

				tsize				  = Z_STRLEN_P(tmp)+1;

				container->scheme[container->size].original_name		= (char*)emalloc(sizeof(char*) * tsize);
				container->scheme[container->size].original_name_len	= tsize;

				memcpy(container->scheme[container->size].original_name, Z_STRVAL_P(tmp), tsize);
				container->scheme[container->size].original_name[tsize] = '\0';

				container->scheme[container->size].name		= (char*)emalloc(sizeof(char*) * tsize);
				container->scheme[container->size].name_len	= tsize;

				memcpy(container->scheme[container->size].name, Z_STRVAL_P(tmp), tsize);
				container->scheme[container->size].name[tsize] = '\0';
				php_strtolower(container->scheme[container->size].name, tsize);
				container->scheme[container->size].name_h = zend_inline_hash_func(container->scheme[container->size].name, tsize);

				if (strcmp(container->scheme[container->size].name, container->scheme[container->size].original_name) == 0) {
					// use snake case?
					container->scheme[container->size].magic_type = 0;
				} else {
					container->scheme[container->size].magic_type = 1;
				}

				zend_mangle_property_name(&mangle, &mangle_len, (char*)"*", 1, (char*)container->scheme[container->size].original_name, container->scheme[container->size].original_name_len, 0);
				container->scheme[container->size].mangled_name	 = mangle;
				container->scheme[container->size].mangled_name_len = mangle_len;
				container->scheme[container->size].mangled_name_h = zend_inline_hash_func(mangle, mangle_len);
				container->scheme[container->size].skip = 0;
			}

			php_pb_field_descriptor_get_property(Z_OBJPROP_P(descriptor), "required", sizeof("required"), &tmp TSRMLS_CC);
			if (Z_TYPE_P(tmp) == IS_BOOL) {
				convert_to_long(tmp);
				container->scheme[container->size].required = Z_LVAL_P(tmp);
			}

			php_pb_field_descriptor_get_property(Z_OBJPROP_P(descriptor), "optional", sizeof("optional"), &tmp TSRMLS_CC);
			if (Z_TYPE_P(tmp) == IS_BOOL) {
				convert_to_long(tmp);
				container->scheme[container->size].optional = Z_LVAL_P(tmp);
			}

			php_pb_field_descriptor_get_property(Z_OBJPROP_P(descriptor), "repeated", sizeof("repeated"), &tmp TSRMLS_CC);
			if (Z_TYPE_P(tmp) == IS_BOOL) {
				convert_to_long(tmp);
				container->scheme[container->size].repeated = Z_LVAL_P(tmp);
			}

			php_pb_field_descriptor_get_property(Z_OBJPROP_P(descriptor), "packable", sizeof("packable"), &tmp TSRMLS_CC);
			if (Z_TYPE_P(tmp) == IS_BOOL) {
				convert_to_long(tmp);
				container->scheme[container->size].packed = Z_LVAL_P(tmp);
			}

			if (container->scheme[container->size].type == TYPE_MESSAGE) {
				zend_class_entry **c;

				php_pb_field_descriptor_get_property(Z_OBJPROP_P(descriptor), "message", sizeof("message"), &tmp TSRMLS_CC);
				if (Z_TYPE_P(tmp) == IS_STRING) {
					if (zend_lookup_class(Z_STRVAL_P(tmp), Z_STRLEN_P(tmp), &c TSRMLS_CC) == FAILURE) {
						zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "the class %s does not find.", Z_STRVAL_P(tmp));
						return;
					}

					container->scheme[container->size].ce = *c;
				} else {
					zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "message wiretype set. we need message parameter for referencing class entry.");
					return;
				}
			}

			container->size++;
			qsort(container->scheme, container->size, sizeof(pb_scheme), sort_cb);
		}
	}
}
/* }}} */

int php_protocolbuffers_extension_registry_get_registry(zval *instance, const char* message_class, size_t message_class_len, zval **result TSRMLS_DC)
{
	zval **bucket;
	php_protocolbuffers_extension_registry *registry;
	registry = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_extension_registry, instance);

	if (zend_hash_find(registry->registry, message_class, message_class_len, (void **)&bucket) == SUCCESS) {
		*result = *bucket;
		return 1;
	}

	return 0;
}

int php_protocolbuffers_extension_registry_get_descriptor_by_name(zval *hash, const char* name, size_t name_len, zval **result TSRMLS_DC)
{
	zval **bucket, **bucket2;

	if (zend_hash_find(Z_ARRVAL_P(hash), "map", sizeof("map"), (void **)&bucket) == SUCCESS) {
		if (zend_hash_find(Z_ARRVAL_PP(bucket), name, name_len+1, (void **)&bucket2) == SUCCESS) {
			*result = *bucket2;
			return 1;
		}
	}

	return 0;
}


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
	protocol_buffers_extension_registry_class_entry->ce_flags |= ZEND_ACC_FINAL_CLASS;

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "ExtensionRegistry", protocol_buffers_extension_registry_class_entry);
}
