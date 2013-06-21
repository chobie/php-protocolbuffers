#include "php_protocol_buffers.h"
#include "message.h"
#include "ext/standard/php_smart_str.h"

static zend_object_handlers php_protocolbuffers_message_object_handlers;

#define PHP_PB_MESSAGE_CHECK_SCHEME \
	{\
		ce = Z_OBJCE_P(instance);\
		err = pb_get_scheme_container(ce->name, ce->name_length, &container, proto TSRMLS_CC);\
		if (err) {\
			if (EG(exception)) {\
				return;\
			} else {\
				/* TODO: improve displaying error message */\
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_get_scheme_cointainer failed. %s does not have getDescriptor method", ce->name);\
				return;\
			}\
		}\
	}


static void php_protocolbuffers_message_free_storage(php_protocolbuffers_message *object TSRMLS_DC)
{
	zend_object_std_dtor(&object->zo TSRMLS_CC);
	efree(object);
}

zend_object_value php_protocolbuffers_message_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	PHP_PROTOCOLBUFFERS_STD_CREATE_OBJECT(php_protocolbuffers_message);

	object->max    = 0;
	object->offset = 0;

	retval.handlers = &php_protocolbuffers_message_object_handlers;

	return retval;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_message_serialize_to_string, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_message_parse_from_string, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_message___call, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_message_merge_from, 0, 0, 1)
	ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()

/* {{{ proto mixed ProtocolBuffersMessage::serializeToString()
*/
PHP_METHOD(protocolbuffers_message, serializeToString)
{
	zval *instance = getThis();

	php_protocolbuffers_encode(INTERNAL_FUNCTION_PARAM_PASSTHRU, Z_OBJCE_P(instance), instance);
}
/* }}} */

/* {{{ proto mixed ProtocolBuffersMessage::parseFromString()
*/
PHP_METHOD(protocolbuffers_message, parseFromString)
{
#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 3)
	/* I tried to lookup current scope from EG(current_execute_data). but it doesn't have current scope. we can't do anymore */
	zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "ProtocolBuffersMessage::parseFromString can't work under PHP 5.3. please use ProtocolBuffers:decode directly");
	return;
#else
	const char *data;
	int data_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"s", &data, &data_len) == FAILURE) {
		return;
	}
	if (EG(called_scope)) {
		php_protocolbuffers_decode(INTERNAL_FUNCTION_PARAM_PASSTHRU, data, data_len, EG(called_scope)->name, EG(called_scope)->name_length);
	} else {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Missing EG(current_scope). this is bug");
	}

#endif
}
/* }}} */

static void php_protocolbuffers_get_hash(pb_scheme_container *container, pb_scheme *scheme, zval *object, char **name, int *name_len, HashTable **ht TSRMLS_DC)
{
	char *n;
	int n_len;
	HashTable *htt;

	if (container->use_single_property > 0) {
		n     = container->single_property_name;
		n_len = container->single_property_name_len;

		if (zend_hash_find(Z_OBJPROP_P(object), n, n_len, (void **)&htt) == FAILURE) {
			return;
		}
	} else {
		htt = Z_OBJPROP_P(object);

		n = scheme->mangled_name;
		n_len = scheme->mangled_name_len;
	}

	name = &n;
	name_len = &n_len;
	ht = &htt;

}

/* {{{ proto mixed ProtocolBuffersMessage::mergeFrom($message)
*/
PHP_METHOD(protocolbuffers_message, mergeFrom)
{
	zval *instance = getThis();
	zval *object;
	int data_len = 0;
	php_protocolbuffers_message *from, *current;
	pb_scheme_container *container;
	pb_scheme *scheme;
	zend_class_entry *ce;
	HashTable *proto = NULL;
	char *n;
	int n_len;
	HashTable *htt;
	int err;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"z", &object) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "%s::mergeFrom expects %s class", Z_OBJCE_P(instance)->name, Z_OBJCE_P(instance)->name);
		return;
	}

	if (Z_OBJCE_P(object) != Z_OBJCE_P(instance)) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "%s::mergeFrom expects %s class, but %s given", Z_OBJCE_P(instance)->name, Z_OBJCE_P(instance)->name, Z_OBJCE_P(object)->name);
		return;
	}

	PHP_PB_MESSAGE_CHECK_SCHEME
	from = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_message, instance);

	php_protocolbuffers_get_hash(container, scheme, object, &n, &n_len, &htt TSRMLS_CC);

	{
		int i = 0;
		for (i = 0; i < container->size; i++) {
			scheme = &(container->scheme[i]);
			fprintf(stderr, "name: %s\n", scheme->name);
		}
	}
}
/* }}} */


/* {{{ proto mixed ProtocolBuffersMessage::current()
*/
PHP_METHOD(protocolbuffers_message, current)
{
	zval *instance = getThis();
	int err = 0;
	zend_class_entry *ce;
	HashTable *proto = NULL;
	pb_scheme_container *container;
	php_protocolbuffers_message *message;
	const char *name;
	int name_len = 0;
	zval **tmp;
	HashTable *hash;

	PHP_PB_MESSAGE_CHECK_SCHEME
	message = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_message, instance);

	if (container->use_single_property < 1) {
		name     = container->scheme[message->offset].mangled_name;
		name_len = container->scheme[message->offset].mangled_name_len;

		hash = Z_OBJPROP_P(instance);
	} else {
		zval **c;

		name     = container->scheme[message->offset].name;
		name_len = container->scheme[message->offset].name_len;

		if (zend_hash_find(Z_OBJPROP_P(instance), container->single_property_name, container->single_property_name_len+1, (void**)&c) == SUCCESS) {
			hash = Z_ARRVAL_PP(c);
		}

		hash = Z_OBJPROP_PP(c);
	}

	if (zend_hash_find(hash, name, name_len, (void **)&tmp) == SUCCESS) {
    	RETURN_ZVAL(*tmp, 1, 0);
	}
}
/* }}} */

/* {{{ proto mixed ProtocolBuffersMessage::key()
*/
PHP_METHOD(protocolbuffers_message, key)
{
	zval *instance = getThis();
	int err = 0;
	zend_class_entry *ce;
	HashTable *proto = NULL;
	pb_scheme_container *container;
	php_protocolbuffers_message *message;

	PHP_PB_MESSAGE_CHECK_SCHEME
	message = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_message, instance);

	RETURN_STRING(container->scheme[message->offset].name, 1);
}
/* }}} */

/* {{{ proto mixed ProtocolBuffersMessage::next()
*/
PHP_METHOD(protocolbuffers_message, next)
{
	zval *instance = getThis();
	int err = 0;
	zend_class_entry *ce;
	HashTable *proto = NULL;
	pb_scheme_container *container;
	php_protocolbuffers_message *message;

	PHP_PB_MESSAGE_CHECK_SCHEME
	message = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_message, instance);

	message->offset++;
}
/* }}} */

/* {{{ proto void ProtocolBuffersMessage::rewind()
*/
PHP_METHOD(protocolbuffers_message, rewind)
{
	zval *instance = getThis();
	int err = 0;
	zend_class_entry *ce;
	HashTable *proto = NULL;
	pb_scheme_container *container;
	php_protocolbuffers_message *message;

	PHP_PB_MESSAGE_CHECK_SCHEME
	message = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_message, instance);

	if (message->max == 0) {
		message->max = container->size;
	}

	message->offset = 0;
}
/* }}} */

/* {{{ proto bool ProtocolBuffersMessage::valid()
*/
PHP_METHOD(protocolbuffers_message, valid)
{
	zval *instance = getThis();
	int err = 0;
	zend_class_entry *ce;
	HashTable *proto = NULL;
	pb_scheme_container *container;
	php_protocolbuffers_message *message;

	PHP_PB_MESSAGE_CHECK_SCHEME
	message = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_message, instance);

	if (message->offset < message->max) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool ProtocolBuffersMessage::__call()
*/
PHP_METHOD(protocolbuffers_message, __call)
{
	zval *instance = getThis();
	int err = 0;
	zend_class_entry *ce;
	HashTable *proto = NULL;
	pb_scheme_container *container;
	php_protocolbuffers_message *message;
	char *name;
	int name_len;
	zval *params;
	int i = 0;
	int flag = 0;
	smart_str buf = {0};
	smart_str buf2 = {0};
	char *n;
	int n_len;
	pb_scheme *scheme;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"sz", &name, &name_len, &params) == FAILURE) {
		return;
	}

	for (i = 0; i < name_len; i++) {
		if (flag == 0) {
			if (i+2 < name_len && name[i] == 'g' && name[i+1] == 'e' && name[i+2] == 't') {
				i += 2;
				flag = 1;
				continue;
			} else if (i+2 < name_len && name[i] == 's' && name[i+1] == 'e' && name[i+2] == 't') {
				i += 2;
				flag = 2;
				continue;
			} else if (i+6 < name_len &&
				name[i] == 'a' &&
				name[i+1] == 'p' &&
				name[i+2] == 'p' &&
				name[i+3] == 'e' &&
				name[i+4] == 'n' &&
				name[i+5] == 'd'
			) {
				i += 6;
				flag = 3;
			} else if (i+5 < name_len &&
				name[i] == 'c' &&
				name[i+1] == 'l' &&
				name[i+2] == 'e' &&
				name[i+3] == 'a' &&
				name[i+4] == 'r') {
				i += 5;
				flag = 4;
			} else if (i+3 < name_len &&
				name[i] == 'h' &&
				name[i+1] == 'a' &&
				name[i+2] == 's'
			) {
				i += 3;
				flag = 5;
			} else {
				break;
			}
		}

		if (name[i] >= 'A' && name[i] <= 'Z') {
			if (buf.len > 1) {
				smart_str_appendc(&buf, '_');
			}
			smart_str_appendc(&buf, name[i] + ('a' - 'A'));
			smart_str_appendc(&buf2, name[i]);
		} else {
			smart_str_appendc(&buf, name[i]);
			smart_str_appendc(&buf2, name[i]);
		}
	}
	smart_str_0(&buf);
	smart_str_0(&buf2);

	if (flag == 0) {
		smart_str_free(&buf);
		smart_str_free(&buf2);
		zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJCE_P(instance)->name, name);
		return;
	}

	PHP_PB_MESSAGE_CHECK_SCHEME
	message = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_message, instance);

	for (i = 0; i < container->size; i++) {
		scheme = &container->scheme[i];

		if (strcmp(scheme->name, buf.c) == 0) {
			// OK
			break;
		}
		if (scheme->magic_type == 1 && strcasecmp(scheme->original_name, buf2.c) == 0) {
			break;
		}
		scheme = NULL;
	}
	smart_str_free(&buf);
	smart_str_free(&buf2);

	if (scheme == NULL) {
		smart_str_free(&buf);
		smart_str_free(&buf2);
		zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJCE_P(instance)->name, name);
		return;
	}

	{
		HashTable *htt = NULL;
		zval **e = NULL;
		zval *vl = NULL;

		if (container->use_single_property > 0) {
			n     = container->single_property_name;
			n_len = container->single_property_name_len;

			if (zend_hash_find(Z_OBJPROP_P(instance), n, n_len, (void **)&htt) == FAILURE) {
				return;
			}
		} else {
			htt = Z_OBJPROP_P(instance);

			n = scheme->mangled_name;
			n_len = scheme->mangled_name_len;
		}

		switch (flag) {
			case 1: /* get */
				if (zend_hash_find(htt, n, n_len, (void **)&e) == SUCCESS) {
					RETVAL_ZVAL(*e, 1, 0);
				}
			break;
			case 2: /* set */
				if (scheme->repeated == 0) {
					if (zend_hash_find(htt, n, n_len, (void **)&e) == SUCCESS) {
						zval **tmp;
						//zval_ptr_dtor(e);

						zend_hash_get_current_data(Z_ARRVAL_P(params), (void **)&tmp);
						MAKE_STD_ZVAL(vl);

						ZVAL_ZVAL(vl, *tmp, 1, 0);

						/* TODO: message type check */
						switch (scheme->type) {
							case TYPE_STRING:
								if (Z_TYPE_P(vl) != IS_STRING) {
									convert_to_string(vl);
								}
							break;
							case TYPE_SINT32:
							case TYPE_INT32:
							case TYPE_UINT32:
								if (Z_TYPE_P(vl) != IS_LONG) {
									convert_to_long(vl);
								}
							break;
							case TYPE_SINT64:
							case TYPE_INT64:
							case TYPE_UINT64:
								if (Z_TYPE_P(vl) != IS_LONG) {
									convert_to_long(vl);
								}
							break;
						}

						zend_hash_update(htt, n, n_len, (void **)&vl, sizeof(zval), NULL);
					}
				} else {
					if (zend_hash_find(htt, n, n_len, (void **)&e) == SUCCESS) {
						zval **tmp;
						zval_ptr_dtor(e);

						zend_hash_get_current_data(Z_ARRVAL_P(params), (void **)&tmp);

						if (Z_TYPE_PP(tmp) != IS_ARRAY) {
							zend_error_noreturn(E_ERROR, "Can't call method %s::%s(). only accept array parameter for repeated fields", Z_OBJCE_P(instance)->name, name);
						}

						MAKE_STD_ZVAL(vl);
						ZVAL_ZVAL(vl, *tmp, 1, 0);
						Z_ADDREF_P(vl);

						zend_hash_update(htt, n, n_len, (void **)&vl, sizeof(zval), NULL);
					}
					return;
				}
			break;
			case 3: /* append */
				if (scheme->repeated > 0) {
					if (zend_hash_find(htt, n, n_len, (void **)&e) == SUCCESS) {
						zval **tmp = NULL;
						zval *nval = NULL;
						zval *xval = NULL;
						int flag = 0;

						if (zend_hash_num_elements(Z_ARRVAL_PP(e)) == 0) {
							MAKE_STD_ZVAL(nval);
							array_init(nval);
							flag = 1;
						} else {
							nval = *e;
						}
						zend_hash_get_current_data(Z_ARRVAL_P(params), (void **)&tmp);

						Z_ADDREF_P(*tmp);
						Z_ADDREF_P(nval);
						zend_hash_next_index_insert(Z_ARRVAL_P(nval), tmp, sizeof(zval *), NULL);
						zend_hash_update(htt, n, n_len, (void **)&nval, sizeof(zval *), NULL);
						zval_ptr_dtor(&tmp);

						if (flag == 1) {
							zval_ptr_dtor(&nval);
						}
					}
				} else {
					zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJCE_P(instance)->name, name);
					return;
				}
			break;
			case 4:  /* clear */
				if (zend_hash_find(htt, n, n_len, (void **)&e) == SUCCESS) {
					zval *t;
					MAKE_STD_ZVAL(t);
					ZVAL_NULL(t);

					zend_hash_update(htt, n, n_len, (void **)&t, sizeof(zval), NULL);
				}
			case 5:  /* has*/
				if (zend_hash_find(htt, n, n_len, (void **)&e) == SUCCESS) {
					if (Z_TYPE_PP(e) == IS_NULL) {
						RETURN_FALSE;
					} else if (Z_TYPE_PP(e) == IS_ARRAY) {
						if (zend_hash_num_elements(Z_ARRVAL_PP(e)) < 1) {
							RETURN_FALSE;
						} else {
							RETURN_TRUE;
						}
					} else {
						RETURN_TRUE;
					}
				}
			break;
		}
	}
}
/* }}} */

static zend_function_entry php_protocolbuffers_message_methods[] = {
	PHP_ME(protocolbuffers_message, serializeToString, arginfo_pb_message_serialize_to_string, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_message, parseFromString, arginfo_pb_message_parse_from_string, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(protocolbuffers_message, mergeFrom, arginfo_pb_message_merge_from, ZEND_ACC_PUBLIC)
	/* iterator */
	PHP_ME(protocolbuffers_message, current,   NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_message, key,       NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_message, next,      NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_message, rewind,    NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_message, valid,     NULL, ZEND_ACC_PUBLIC)
	/* magic method */
	PHP_ME(protocolbuffers_message, __call,   arginfo_pb_message___call, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void php_pb_message_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersMessage", php_protocolbuffers_message_methods);
	protocol_buffers_message_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(protocol_buffers_message_class_entry TSRMLS_CC, 1, zend_ce_iterator);
	zend_class_implements(protocol_buffers_message_class_entry TSRMLS_CC, 1, protocol_buffers_serializable_class_entry);

	protocol_buffers_message_class_entry->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
	//protocol_buffers_message_class_entry->create_object = php_protocolbuffers_message_new;

	memcpy(&php_protocolbuffers_message_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "Message", protocol_buffers_message_class_entry);
}
