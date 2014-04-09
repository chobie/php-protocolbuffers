#include "protocolbuffers.h"
#include "serializer.h"
#include "json_serializer.h"

int php_protocolbuffers_encode_jsonserialize(INTERNAL_FUNCTION_PARAMETERS, zval *klass, php_protocolbuffers_scheme_container *container, zval **result)
{
	int i = 0;
	php_protocolbuffers_scheme *scheme;
	HashTable *hash = NULL;
	zval **c = NULL;
	zval *target = *result;

	if (container->use_single_property < 1) {
		hash = Z_OBJPROP_P(klass);
	} else {
		if (zend_hash_find(Z_OBJPROP_P(klass), container->single_property_name, container->single_property_name_len+1, (void**)&c) == SUCCESS) {
			hash = Z_ARRVAL_PP(c);
		} else {
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "the class does not have `_properties` protected property.");
			return -1;
		}
	}

	for (i = 0; i < container->size; i++) {
		zval *tmp;
		scheme = &(container->scheme[i]);

		if (php_protocolbuffers_fetch_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, &tmp)) {
			return -1;
		}

		switch (scheme->type) {
			case TYPE_DOUBLE:
			case TYPE_FLOAT:
			case TYPE_INT64:
			case TYPE_UINT64:
			case TYPE_INT32:
			case TYPE_FIXED64:
			case TYPE_FIXED32:
			case TYPE_BOOL:
			case TYPE_STRING:
			case TYPE_GROUP:
			case TYPE_BYTES:
			case TYPE_UINT32:
			case TYPE_ENUM:
			case TYPE_SFIXED32:
			case TYPE_SFIXED64:
			case TYPE_SINT32:
			case TYPE_SINT64:
				if (Z_TYPE_P(tmp) != IS_NULL) {
					if (scheme->repeated) {
						zval *out;
						HashPosition pos;
						zval **entry, *tt;
						zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(tmp), &pos);

						if (zend_hash_num_elements(Z_ARRVAL_P(tmp)) > 0) {
							MAKE_STD_ZVAL(tt);
							array_init(tt);
							while (zend_hash_get_current_data_ex(Z_ARRVAL_P(tmp), (void **)&entry, &pos) == SUCCESS) {
								MAKE_STD_ZVAL(out);
								array_init(out);

								// TODO(chobie): check types
								add_next_index_zval(tt, out);
								zend_hash_move_forward_ex(Z_ARRVAL_P(tmp), &pos);
							}
							add_assoc_zval(target, scheme->name, tt);
						}

					} else {
						if (Z_TYPE_P(tmp) == IS_STRING) {
							if (Z_STRLEN_P(tmp) == 0 || (Z_STRLEN_P(tmp) == 1 && Z_STRVAL_P(tmp)[0] == '0')) {
							} else {
									Z_ADDREF_P(tmp);

									// TODO(chobie): check types
									add_assoc_zval(target, scheme->name, tmp);
							}
						} else if (Z_TYPE_P(tmp) == IS_NULL) {
							// Nothing to do
						} else {
							Z_ADDREF_P(tmp);
							add_assoc_zval(target, scheme->name, tmp);
						}
					}
				}

			break;
			case TYPE_MESSAGE: {
				zval *out;
				php_protocolbuffers_scheme_container *c2;
				php_protocolbuffers_get_scheme_container(scheme->ce->name, scheme->ce->name_length, &c2 TSRMLS_CC);

				if (scheme->repeated) {
					HashPosition pos;
					zval **entry, *tt;
					zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(tmp), &pos);

					if (zend_hash_num_elements(Z_ARRVAL_P(tmp)) > 0) {
						MAKE_STD_ZVAL(tt);
						array_init(tt);
						while (zend_hash_get_current_data_ex(Z_ARRVAL_P(tmp), (void **)&entry, &pos) == SUCCESS) {
							MAKE_STD_ZVAL(out);
							array_init(out);

							php_protocolbuffers_encode_jsonserialize(INTERNAL_FUNCTION_PARAM_PASSTHRU, *entry, c2, &out);

							add_next_index_zval(tt, out);
							zend_hash_move_forward_ex(Z_ARRVAL_P(tmp), &pos);
						}
						add_assoc_zval(target, scheme->name, tt);
					}
				} else {
					if (Z_TYPE_P(tmp) != IS_NULL) {
						MAKE_STD_ZVAL(out);
						array_init(out);
						php_protocolbuffers_encode_jsonserialize(INTERNAL_FUNCTION_PARAM_PASSTHRU, tmp, c2, &out);
						add_assoc_zval(target, scheme->name, out);
					}
				}

				break;
			}
			default:
			break;
		}

		if (EG(exception)) {
			return 1;
		}
	}

	return 0;
}
