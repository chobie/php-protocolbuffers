#ifndef PHP_PROTOCOLBUFFERS_UNKNOWN_FIELD_SET_H
#define PHP_PROTOCOLBUFFERS_UNKNOWN_FIELD_SET_H

void php_pb_unknown_field_set_class(TSRMLS_D);


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

static void php_pb_unknown_field_set_add_field(INTERNAL_FUNCTION_PARAMETERS, zval *instance, int number, char *name, int name_len, zval *field)
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
				//efree(name);
				//zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "number %d has already added", number);
				return;
			}
		}
	}

	zend_hash_next_index_insert(Z_ARRVAL_PP(fields), (void *)&field, sizeof(zval *), NULL);
	//Z_ADDREF_P(field);
	efree(name);
}

#undef PHP_PB_INITIALIZE_FIELDS
#endif