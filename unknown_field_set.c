#include "php_protocol_buffers.h"
#include "unknown_field_set.h"

#define PHP_PB_INITIALIZE_FIELDS \
		if (Z_TYPE_PP(fields) != IS_ARRAY) {\
			zval *tmp;\
\
			MAKE_STD_ZVAL(tmp);\
			array_init(tmp);\
\
			zend_hash_update(Z_OBJPROP_P(instance), name, name_len, (void **)&tmp, sizeof(zval*), NULL);\
			fields = &tmp;\
		}\

int php_pb_unknown_field_get_field(INTERNAL_FUNCTION_PARAMETERS, zval *instance, int number, char *name, int name_len, php_protocolbuffers_unknown_field **f)
{
	HashPosition pos;
	HashTable *fieldht;
	zval **element, **fields;

	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"fields", sizeof("fields"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&fields) == SUCCESS) {
		PHP_PB_INITIALIZE_FIELDS

		fieldht = Z_ARRVAL_PP(fields);
		for(zend_hash_internal_pointer_reset_ex(fieldht, &pos);
						zend_hash_get_current_data_ex(fieldht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(fieldht, &pos)
		) {
			php_protocolbuffers_unknown_field *t = NULL;
			t = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, *element);

			if (t->number == number) {
				*f = t;
				return 1;
			}
		}
	}
	efree(name);
	return 0;
}


void php_pb_unknown_field_set_add_field(INTERNAL_FUNCTION_PARAMETERS, zval *instance, int number, char *name, int name_len, zval *field)
{
	HashPosition pos;
	HashTable *fieldht;
	zval **element, **fields;

	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"fields", sizeof("fields"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&fields) == SUCCESS) {
		PHP_PB_INITIALIZE_FIELDS

		fieldht = Z_ARRVAL_PP(fields);
		for(zend_hash_internal_pointer_reset_ex(fieldht, &pos);
						zend_hash_get_current_data_ex(fieldht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(fieldht, &pos)
		) {
			php_protocolbuffers_unknown_field *s = NULL, *t = NULL;
			s = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, field);
			t = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, *element);

			if (t->number == number) {
				//efree(name);
				//zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "number %d has already added", number);
				return;
			}
		}
	}

	zend_hash_next_index_insert(Z_ARRVAL_PP(fields), (void *)&field, sizeof(zval *), NULL);
	efree(name);
}

#undef PHP_PB_INITIALIZE_FIELDS

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_set___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_set_count, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_get_field, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_unknown_field_add_field, 0, 0, 1)
	ZEND_ARG_INFO(0, unknown_field)
ZEND_END_ARG_INFO()

#define PHP_PB_INITIALIZE_FIELDS \
		if (Z_TYPE_PP(fields) != IS_ARRAY) {\
			zval *tmp;\
\
			MAKE_STD_ZVAL(tmp);\
			array_init(tmp);\
\
			zend_hash_update(Z_OBJPROP_P(instance), name, name_len, (void **)&tmp, sizeof(zval*), NULL);\
			fields = &tmp;\
		}\


/* {{{ proto void ProtocolBuffersUnknownFieldSet::count()
*/
PHP_METHOD(protocolbuffers_unknown_field_set, count)
{
	char *name;
	int name_len;
	long num = 0;
	zval *instance = getThis();
	zval **fields;

	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"fields", sizeof("fields"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&fields) == SUCCESS) {
		PHP_PB_INITIALIZE_FIELDS

		num = zend_hash_num_elements(Z_ARRVAL_PP(fields));
	}
	efree(name);

	RETURN_LONG(num);
}
/* }}} */

/* {{{ proto ProtocolBuffersUnknownField ProtocolBuffersUnknownFieldSet::getField(int $index)
*/
PHP_METHOD(protocolbuffers_unknown_field_set, getField)
{
	zval *instance = getThis();
	zval **fields = NULL, *result = NULL;
	long tag;
	char *name;
	int name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &tag) == FAILURE) {
		return;
	}

	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"fields", sizeof("fields"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&fields) == SUCCESS) {
		HashPosition pos;
		HashTable *fieldht;
		zval **element;

		PHP_PB_INITIALIZE_FIELDS

		fieldht = Z_ARRVAL_PP(fields);

		for(zend_hash_internal_pointer_reset_ex(fieldht, &pos);
						zend_hash_get_current_data_ex(fieldht, (void **)&element, &pos) == SUCCESS;
						zend_hash_move_forward_ex(fieldht, &pos)
		) {
			php_protocolbuffers_unknown_field *t = NULL;
			t = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, *element);

			if (t->number == tag) {
				result = *element;
				break;
			}
		}
	}
	efree(name);

	if (result != NULL) {
		RETURN_ZVAL(result, 1, 0);
	}
}
/* }}} */

/* {{{ proto void ProtocolBuffersUnknownFieldSet::addField(ProtocolBuffersUnknownField $field)
*/
PHP_METHOD(protocolbuffers_unknown_field_set, addField)
{
	zval *instance = getThis();
	zval *field = NULL;
	char *name = {0};
	int name_len = 0;
	php_protocolbuffers_unknown_field *a = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"O", &field, protocol_buffers_unknown_field_class_entry) == FAILURE) {
		return;
	}
	a = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, field);

	php_pb_unknown_field_set_add_field(INTERNAL_FUNCTION_PARAM_PASSTHRU, instance, a->number, name, name_len, field);
}
/* }}} */


static zend_function_entry php_protocolbuffers_unknown_field_set_methods[] = {
	PHP_ME(protocolbuffers_unknown_field_set, count, arginfo_pb_unknown_field_set_count, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field_set, getField, arginfo_pb_unknown_field_get_field, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_unknown_field_set, addField, arginfo_pb_unknown_field_add_field, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void php_pb_unknown_field_set_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersUnknownFieldSet", php_protocolbuffers_unknown_field_set_methods);
	protocol_buffers_unknown_field_set_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	zend_declare_property_null(protocol_buffers_unknown_field_set_class_entry, "fields", sizeof("fields")-1, ZEND_ACC_PROTECTED TSRMLS_CC);

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "UnknownFieldSet", protocol_buffers_unknown_field_set_class_entry);
}

#undef PHP_PB_INITIALIZE_FIELDS