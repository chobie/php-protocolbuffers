#include "php_protocol_buffers.h"
#include "ext/spl/spl_iterators.h"
#include "message.h"


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

	return retval;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_message_serialize_to_string, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_message_parse_from_string, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

/* {{{ proto mixed ProtocolBuffersMessage::serializeToString()
*/
PHP_METHOD(protocolbuffers_message, serializeToString)
{
	/* TODO: refactor this block. please see protocol_buffers.c too */
	zval *instance = getThis();
	zend_class_entry *ce;
	pb_scheme_container *container;
	HashTable *proto = NULL;
	pb_serializer *ser = NULL;
	int err = 0;

	PHP_PB_MESSAGE_CHECK_SCHEME

	if (pb_encode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, instance, container, &ser)) {
		RETURN_EMPTY_STRING();
	}

	if (ser == NULL) {
		RETURN_EMPTY_STRING();
	}

	if (ser->buffer_size > 0) {
		RETVAL_STRINGL((char*)ser->buffer, ser->buffer_size, 1);

		pb_serializer_destroy(ser);
	} else {
		pb_serializer_destroy(ser);
		RETURN_EMPTY_STRING();
	}
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
	const char *data, *data_end, *res;
	int data_len = 0;
	int err = 0;
	long buffer_size = 0;
	pb_scheme_container *container;
	zval *obj = NULL;


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"s", &data, &data_len) == FAILURE) {
		return;
	}
	if (EG(called_scope)) {
		/* TODO: refactor this block. please see protocol_buffers.c too */
		buffer_size = (long)data + sizeof(data);

		err = pb_get_scheme_container(EG(called_scope)->name, EG(called_scope)->name_length, &container, NULL TSRMLS_CC);
		if (err) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_get_scheme_container failed. %s does not have getDescriptor method", EG(called_scope)->name);
			return;
		}

		data_end = data + data_len;

		{
			zend_class_entry **ce;

			MAKE_STD_ZVAL(obj);
			zend_lookup_class(EG(called_scope)->name, EG(called_scope)->name_length, &ce TSRMLS_CC);
			object_init_ex(obj, *ce);

			/* add unknown fields */
			{
				zval *unknown;
				zval **un;
				char *unknown_name;
				int unknown_name_len;

				MAKE_STD_ZVAL(unknown);

				object_init_ex(unknown, protocol_buffers_unknown_field_set_class_entry);
				zend_mangle_property_name(&unknown_name, &unknown_name_len, (char*)"*", 1, (char*)"_unknown", sizeof("_unknown"), 0);
				if (zend_hash_find(Z_OBJPROP_P(obj), (char*)unknown_name, unknown_name_len, (void **)&un) == FAILURE) {
					zend_hash_update(Z_OBJPROP_P(obj), unknown_name, unknown_name_len, (void **)&unknown, sizeof(unknown), NULL);
				} else {
					if (Z_TYPE_PP(un) == IS_NULL) {
						zend_hash_update(Z_OBJPROP_P(obj), unknown_name, unknown_name_len, (void **)&unknown, sizeof(unknown), NULL);
					} else {
						efree(unknown);
					}
				}
				efree(unknown_name);
			}

			res = pb_decode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, data, data_end, container, &obj);
			if (res == NULL) {
				zval_ptr_dtor(&obj);
				zend_throw_exception_ex(protocol_buffers_invalid_protocolbuffers_exception_class_entry, 0 TSRMLS_CC, "passed variable contains malformed byte sequence. or it contains unsupported tag");
				return;
			}

			/* TODO:
			pb_execute_wakeup(obj TSRMLS_CC);
			*/
		}

		RETURN_ZVAL(obj, 0, 1);
	} else {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Missing EG(current_scope). this is bug");
	}

#endif
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

static zend_function_entry php_protocolbuffers_message_methods[] = {
	PHP_ME(protocolbuffers_message, serializeToString, arginfo_pb_message_serialize_to_string, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_message, parseFromString, arginfo_pb_message_parse_from_string, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	/* iterator */
	PHP_ME(protocolbuffers_message, current,   NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_message, key,       NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_message, next,      NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_message, rewind,    NULL, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_message, valid,     NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void php_pb_message_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersMessage", php_protocolbuffers_message_methods);
	protocol_buffers_message_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(protocol_buffers_message_class_entry TSRMLS_CC, 1, spl_ce_Iterator);
	zend_class_implements(protocol_buffers_message_class_entry TSRMLS_CC, 1, protocol_buffers_serializable_class_entry);

	protocol_buffers_message_class_entry->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
	protocol_buffers_message_class_entry->create_object = php_protocolbuffers_message_new;
}
