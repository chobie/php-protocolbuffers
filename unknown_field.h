#ifndef PHP_PROTOCOLBUFFERS_UNKNOWN_FIELD_H
#define PHP_PROTOCOLBUFFERS_UNKNOWN_FIELD_H

void php_pb_unknown_field_class(TSRMLS_D);

static void php_pb_unknown_field_set_number(zval *instance, int number TSRMLS_DC)
{
	php_protocolbuffers_unknown_field *field = NULL;
	char *name;
	int name_len;
	zval **result = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"number", sizeof("number"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&result) == SUCCESS) {
		zval *tmp;

		MAKE_STD_ZVAL(tmp);
		ZVAL_LONG(tmp, number);
		zend_hash_update(Z_OBJPROP_P(instance), name, name_len, (void **)&tmp, sizeof(zval), NULL);
		field->number = number;
	}
	efree(name);
}

static void php_pb_unknown_field_set_type(zval *instance, int type TSRMLS_DC)
{
	php_protocolbuffers_unknown_field *field = NULL;
	char *name;
	int name_len;
	zval **result = NULL;

	field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, instance);
	zend_mangle_property_name(&name, &name_len, (char*)"*", 1, (char*)"type", sizeof("type"), 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), name, name_len, (void **)&result) == SUCCESS) {
		zval *tmp;

		MAKE_STD_ZVAL(tmp);
		ZVAL_LONG(tmp, type);
		zend_hash_update(Z_OBJPROP_P(instance), name, name_len, (void **)&tmp, sizeof(zval), NULL);
		field->type = type;
	}
	efree(name);
}

#endif