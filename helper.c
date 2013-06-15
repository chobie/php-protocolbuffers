#include "php_protocol_buffers.h"
#include "unknown_field_set.h"
#include "unknown_field.h"

void pb_convert_msg(HashTable *proto, const char *klass, int klass_len, pb_scheme **scheme, int *size TSRMLS_DC)
{
	int n, sz;
	zval *element;
	HashPosition pos;
	pb_scheme *ischeme;

	sz = zend_hash_num_elements(proto);
	ischeme = (pb_scheme*)emalloc(sizeof(pb_scheme) * sz);

	for(n = 0, zend_hash_internal_pointer_reset_ex(proto, &pos);
					zend_hash_get_current_data_ex(proto, (void **)&element, &pos) == SUCCESS;
					zend_hash_move_forward_ex(proto, &pos), n++
	) {
		char *key = {0};
		uint  key_len = 0;
		unsigned long index= 0;
		long ttag = 0;
		zend_class_entry **c;

		zend_hash_get_current_key_ex(proto, &key, &key_len, &index, 0, &pos);
		ttag = index;

		ischeme[n].tag = ttag;

		{
			zval *tmp;
			int tsize = 0;
			char *mangle;
			int mangle_len;

			ischeme[n].type = pb_get_lval_from_hash_by_tag(proto, ttag, "type", sizeof("type") TSRMLS_CC);

			pb_get_zval_from_hash_by_tag(proto, ttag, "name", sizeof("name"), &tmp TSRMLS_CC);

			tsize				  = Z_STRLEN_P(tmp)+1;
			ischeme[n].name		= (char*)emalloc(sizeof(char*) * tsize);
			ischeme[n].name_len	= tsize;

			memcpy(ischeme[n].name, Z_STRVAL_P(tmp), tsize);
			ischeme[n].name[tsize] = '\0';

			zend_mangle_property_name(&mangle, &mangle_len, (char*)"*", 1, (char*)ischeme[n].name, ischeme[n].name_len, 0);
			ischeme[n].mangled_name	 = mangle;
			ischeme[n].mangled_name_len = mangle_len;

			ischeme[n].repeated = pb_get_lval_from_hash_by_tag(proto, ttag, "repeated", sizeof("repeated") TSRMLS_CC);
			ischeme[n].packed   = pb_get_lval_from_hash_by_tag(proto, ttag, "packable", sizeof("packable") TSRMLS_CC);

			if (ischeme[n].type == TYPE_MESSAGE) {

				pb_get_zval_from_hash_by_tag(proto, ttag, "message", sizeof("message"), &tmp TSRMLS_CC);

				if (zend_lookup_class(Z_STRVAL_P(tmp), Z_STRLEN_P(tmp), &c TSRMLS_CC) == FAILURE) {
					zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "the class %s does not find.", Z_STRVAL_P(tmp));
					return;
				}
				ischeme[n].ce = *c;
			}
		}
	}

	*scheme = ischeme;
	*size = sz;
}

int pb_get_scheme_container(const char *klass, size_t klass_len, pb_scheme_container **result, HashTable *descriptor TSRMLS_DC)
{
	pb_scheme_container *container, **cn;
	pb_scheme *ischeme;
	int scheme_size = 0;

	if (zend_hash_find(PBG(messages), (char*)klass, klass_len, (void **)&cn) != SUCCESS) {
		zval *ret = NULL;
		HashTable *proto = NULL;


		if (descriptor == NULL) {
			zend_class_entry **ce = NULL;

			if (zend_lookup_class((char*)klass, klass_len, &ce TSRMLS_CC) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_get_scheme_cointainer failed. %s does find", klass);
				return 1;
			}

			if (zend_call_method(NULL, *ce, NULL, "getdescriptor", strlen("getdescriptor"), &ret, 0, NULL, NULL  TSRMLS_CC)) {
				if (Z_TYPE_P(ret) == IS_ARRAY) {
					proto = Z_ARRVAL_P(ret);
				} else if (Z_TYPE_P(ret) == IS_OBJECT) {
					/* TODO: needs refactoring. */
					zend_class_entry *entry;
					php_protocolbuffers_descriptor *desc;

					entry = Z_OBJCE_P(ret);
					if (entry == protocol_buffers_descriptor_class_entry) {
						desc = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_descriptor, ret);
						desc->free_container = 1;
						zend_hash_add(PBG(messages), (char*)klass, klass_len, (void**)&desc->container, sizeof(pb_scheme_container*), NULL);
					} else {
						zend_throw_exception_ex(protocol_buffers_invalid_protocolbuffers_exception_class_entry, 0 TSRMLS_CC, "getDescriptor returns unexpected class");
						if (ret != NULL) {
							zval_ptr_dtor(&ret);
						}
						return 1;
					}

					if (ret != NULL) {
						zval_ptr_dtor(&ret);
					}

					*result = desc->container;

					return 0;
				} else {
					if (ret != NULL) {
						zval_ptr_dtor(&ret);
					}
					zend_throw_exception_ex(protocol_buffers_invalid_protocolbuffers_exception_class_entry, 0 TSRMLS_CC, "passed string is not valid utf8 string");
					return 1;
				}
			} else {
				if (EG(exception)) {
					// do nothing
				} else {
					php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_get_scheme_cointainer failed. %s does not have getDescriptor method", klass);
				}
				return 1;
			}
		} else {
			proto = descriptor;
		}

		pb_convert_msg(proto, klass, klass_len, &ischeme, &scheme_size TSRMLS_CC);
		scheme_size = zend_hash_num_elements(proto);

		container = (pb_scheme_container*)emalloc(sizeof(pb_scheme_container));
		container->scheme = ischeme;
		container->size = scheme_size;
		container->use_single_property = 0;
		container->process_unknown_fields = 0;

		{
			char *prop;
			int prop_len;

			zend_mangle_property_name(&prop, &prop_len, (char*)"*", 1, (char*)"_properties", sizeof("_properties"), 0);
			container->single_property_name = prop;
			container->single_property_name_len = prop_len;
		}

		zend_hash_add(PBG(messages), (char*)klass, klass_len, (void**)&container, sizeof(pb_scheme_container*), NULL);

		if (ret != NULL) {
			zval_ptr_dtor(&ret);
		}
	} else {
		container = *cn;
	}

	*result = container;

	return 0;
}

void process_unknown_field(INTERNAL_FUNCTION_PARAMETERS, pb_scheme_container *container, HashTable *hresult, zval *dz, int tag, int wiretype, int64_t value)
{
	char *unknown_name;
	int unknown_name_len;
	zval **un;

	php_protocolbuffers_unknown_field *p = NULL;

	object_init_ex(dz, protocol_buffers_unknown_field_class_entry);

	p = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, dz);

	p->varint = value;
	php_pb_unknown_field_set_number(dz, tag TSRMLS_CC);
	php_pb_unknown_field_set_type(dz, wiretype TSRMLS_CC);

	if (container->use_single_property > 0) {
		unknown_name = "_unknown";
		unknown_name_len = sizeof("_unknown");
	} else {
		zend_mangle_property_name(&unknown_name, &unknown_name_len, (char*)"*", 1, (char*)"_unknown", sizeof("_unknown"), 0);
	}

	if (zend_hash_find(hresult, (char*)unknown_name, unknown_name_len, (void **)&un) == SUCCESS) {
		php_pb_unknown_field_set_add_field(INTERNAL_FUNCTION_PARAM_PASSTHRU, *un, tag, unknown_name, unknown_name_len, dz);
	}

	if (container->use_single_property < 1) {
		efree(unknown_name);
	}
}

void process_unknown_field_bytes(INTERNAL_FUNCTION_PARAMETERS, pb_scheme_container *container, HashTable *hresult, zval *dz, int tag, int wiretype, uint8_t *bytes, int length)
{
	char *unknown_name;
	int unknown_name_len;
	zval **un;

	php_protocolbuffers_unknown_field *p = NULL;

	object_init_ex(dz, protocol_buffers_unknown_field_class_entry);
	p = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, dz);
	p->buffer = bytes;
	p->buffer_len = length;
	php_pb_unknown_field_set_number(dz, tag TSRMLS_CC);
	php_pb_unknown_field_set_type(dz, wiretype TSRMLS_CC);

	if (container->use_single_property > 0) {
		unknown_name = "_unknown";
		unknown_name_len = sizeof("_unknown");
	} else {
		zend_mangle_property_name(&unknown_name, &unknown_name_len, (char*)"*", 1, (char*)"_unknown", sizeof("_unknown"), 0);
	}

	if (zend_hash_find(hresult, (char*)unknown_name, unknown_name_len, (void **)&un) == SUCCESS) {
		php_pb_unknown_field_set_add_field(INTERNAL_FUNCTION_PARAM_PASSTHRU, *un, tag, unknown_name, unknown_name_len, dz);
	}

	if (container->use_single_property < 1) {
		efree(unknown_name);
	}
}

const char* pb_decode_message(INTERNAL_FUNCTION_PARAMETERS, const char *data, const char *data_end, pb_scheme_container *container, zval **result)
{
	uint32_t value = 0, tag = 0, wiretype = 0;
	zval *dz;
	HashTable *hresult;
	char buffer[512] = {0};

	/* hresult = Z_ARRVAL_PP(result); */
	if (container->use_single_property > 0) {
		zval **tmp = NULL;

		if (zend_hash_find(Z_OBJPROP_PP(result), container->single_property_name, container->single_property_name_len, (void **)&tmp) == SUCCESS) {
			if (Z_TYPE_PP(tmp) == IS_NULL) {
				array_init(*tmp);
			} else if (Z_TYPE_PP(tmp) != IS_ARRAY) {
				zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "single property is not an array.");
				return NULL;
			}

			hresult = Z_ARRVAL_PP(tmp);
		} else {
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "the class does not have property.");
			return NULL;
		}

	} else {
		hresult = Z_OBJPROP_PP(result);
	}

	while (data < data_end) {
		pb_scheme *s = NULL;

		data = ReadVarint32FromArray(data, &value, data_end);

		if (data == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_decode_message failed. ReadVarint32FromArray returns NULL.");
			return NULL;
		}

		tag	  = (value >> 0x03);
		wiretype = (value & 0x07);

		if (tag < 1 || tag > ktagmax) {
			return NULL;
		}
		if (wiretype > 5) {
			return NULL;
		}

		s = pb_search_scheme_by_tag(container->scheme, container->size, tag);

		switch (wiretype) {
		case WIRETYPE_VARINT:
		{
			data = ReadVarint32FromArray(data, &value, data_end);
			MAKE_STD_ZVAL(dz);

			if (s == NULL) {
				if (container->process_unknown_fields > 0) {
					process_unknown_field(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hresult, dz, tag, wiretype, value);
				} else {
					/* skip unknown field */
					zval_ptr_dtor(&dz);
				}
			} else if (s->type == TYPE_BOOL) {
				ZVAL_BOOL(dz, value);
				php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
			} else if (s->type == TYPE_INT32) {
				ZVAL_LONG(dz, (int32_t)value);
				php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
			} else if (s->type == TYPE_SINT32) {
				ZVAL_LONG(dz, (int32_t)zigzag_decode32(value));
				php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
			} else {
				ZVAL_LONG(dz, value);
				php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
			}
		}
		break;
		case WIRETYPE_FIXED64:
			if (s == NULL) {
				if (container->process_unknown_fields > 0) {
					uint64_t l;
					memcpy(&l, data, 8);

					MAKE_STD_ZVAL(dz);
					process_unknown_field(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hresult, dz, tag, wiretype, value);
				} else {
					/* skip unknown field */
				}
			} else if (s->type == TYPE_DOUBLE) {
				uint64_t _v;
				double d;

				memcpy(&_v, data, 8);
				d = decode_double(_v);

				MAKE_STD_ZVAL(dz);
				ZVAL_DOUBLE(dz, d);
				php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
			} else {
				uint64_t l;
				memcpy(&l, data, 8);

				MAKE_STD_ZVAL(dz);
				ZVAL_DOUBLE(dz, l);
				php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
			}

			data += 8;
		break;
		case WIRETYPE_LENGTH_DELIMITED:
			data = ReadVarint32FromArray(data, &value, data_end);

			if ((data+value) > data_end) {
				return NULL;
			}

			if (s == NULL) {
				if (container->process_unknown_fields > 0) {
					uint8_t *bytes_array;
					bytes_array = (uint8_t *)emalloc(value);
					memcpy(bytes_array, data, value);

					MAKE_STD_ZVAL(dz);
					process_unknown_field_bytes(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hresult, dz, tag, wiretype, bytes_array, value);
				} else {
					/* skip unknown field */
				}
			} else if (s->type == TYPE_STRING) {
				if (value < 512) {
					memcpy(buffer, data, value);
					buffer[value] = '\0';

					MAKE_STD_ZVAL(dz);
					ZVAL_STRINGL(dz, (char*)data, value, 1);

					php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
				} else {
					char *sub_buffer;

					sub_buffer = (char *)emalloc(value);
					memcpy(sub_buffer, data, value);

					MAKE_STD_ZVAL(dz);
					ZVAL_STRINGL(dz, (char*)sub_buffer, value, 1);

					php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
					efree(sub_buffer);
				}

			} else if (s->type == TYPE_BYTES) {
				if (value < 512) {
					memcpy(buffer, data, value);
					buffer[value] = '\0';
					MAKE_STD_ZVAL(dz);
					ZVAL_STRINGL(dz, (char*)data, value, 1);

					php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
				} else {
					char *sub_buffer;

					sub_buffer = (char *)emalloc(value);

					memcpy(sub_buffer, data, value);

					MAKE_STD_ZVAL(dz);
					ZVAL_STRINGL(dz, (char*)sub_buffer, value, 1);

					php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);

					efree(sub_buffer);
				}

				data = data + value;
			} else if (s->type == TYPE_MESSAGE) {
				const char *n_buffer_end = data + value;
				zval *z_obj;
				pb_scheme_container *c_container;
				char *name;
				int name_length;

				pb_get_scheme_container(s->ce->name, s->ce->name_length, &c_container, NULL TSRMLS_CC);

				MAKE_STD_ZVAL(z_obj);
				object_init_ex(z_obj, s->ce);

				pb_decode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, data, n_buffer_end, c_container, &z_obj);

				/* TODO: pb_execute_wakeup(z_obj TSRMLS_CC); */

				if (container->use_single_property < 1) {
					name = s->mangled_name;
					name_length = s->mangled_name_len;
				} else {
					name = s->name;
					name_length = s->name_len;
				}

				if (s->repeated) {
					if (!zend_hash_exists(hresult, name, name_length)) {
						zval *arr;

						MAKE_STD_ZVAL(arr);
						array_init(arr);

						zend_hash_next_index_insert(Z_ARRVAL_P(arr), (void *)&z_obj, sizeof(z_obj), NULL);
						Z_ADDREF_P(z_obj);

						zend_hash_update(hresult, name, name_length, (void **)&arr, sizeof(arr), NULL);
						Z_ADDREF_P(arr);
						zval_ptr_dtor(&arr);
					} else {
						zval **arr2;

						if (zend_hash_find(hresult, name, name_length, (void **)&arr2) == SUCCESS) {
							zend_hash_next_index_insert(Z_ARRVAL_PP(arr2), (void *)&z_obj, sizeof(z_obj), NULL);
							Z_ADDREF_P(z_obj);
						}
					}
				} else {
					zend_hash_update(hresult, name, name_length, (void **)&z_obj, sizeof(z_obj), NULL);
					Z_ADDREF_P(z_obj);
				}

				zval_ptr_dtor(&z_obj);
			} else if (s->packed) {
				const char *packed_data_end, *last_data_offset;

				last_data_offset = data;
				packed_data_end = data + value;
				do {
					switch (s->type) {
						case TYPE_DOUBLE:
						{
							uint64_t _v;
							double d;
							memcpy(&_v, data, 8);

							d = decode_double(_v);
							MAKE_STD_ZVAL(dz);
							ZVAL_DOUBLE(dz, d);
							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);

							data += 8;
						}
						break;
						case TYPE_FLOAT:
						{
							uint32_t _v;
							float a = 0;

							memcpy(&_v, data, 4);
							a = decode_float(_v);

							MAKE_STD_ZVAL(dz);
							ZVAL_DOUBLE(dz, a);
							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);

							data += 4;
						}
						break;
						case TYPE_INT64:
						{
							uint64_t v2;
							data = ReadVarint64FromArray(data, &v2, data_end);

							MAKE_STD_ZVAL(dz);
							ZVAL_LONG(dz, (int64_t)v2);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
						}
						break;
						case TYPE_UINT64:
						{
							uint64_t v2;
							data = ReadVarint64FromArray(data, &v2, data_end);

							MAKE_STD_ZVAL(dz);
							ZVAL_LONG(dz, v2);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
						}
						break;
						break;
						case TYPE_INT32:
							data = ReadVarint32FromArray(data, &value, data_end);

							MAKE_STD_ZVAL(dz);
							ZVAL_LONG(dz, (int32_t)value);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
						break;
						case TYPE_FIXED64:
						{
							int64_t l;
							memcpy(&l, data, 8);

							MAKE_STD_ZVAL(dz);
							ZVAL_LONG(dz, l);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
							data += 8;
						}
						break;
						case TYPE_FIXED32:
						{
							uint32_t l = 0;
							memcpy(&l, data, 4);

							MAKE_STD_ZVAL(dz);
#if SIZEOF_LONG == 4
							if (l > 0x7fffffff) {
								/* PHP_INT_MAX is 0x7fffffff on this platform. cast to double. */
								ZVAL_DOUBLE(dz, l);
							} else {
								ZVAL_LONG(dz, (unsigned long)l);
							}
#else
							ZVAL_LONG(dz, (unsigned long)l);
#endif

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);

							data += 4;
						}
						break;
						case TYPE_BOOL:
							data = ReadVarint32FromArray(data, &value, data_end);

							MAKE_STD_ZVAL(dz);
							ZVAL_BOOL(dz, value);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
						break;
						case TYPE_UINT32:
							data = ReadVarint32FromArray(data, &value, data_end);

							MAKE_STD_ZVAL(dz);
							ZVAL_LONG(dz, (int32_t)value);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
						break;
						case TYPE_ENUM:
							data = ReadVarint32FromArray(data, &value, data_end);

							MAKE_STD_ZVAL(dz);
							ZVAL_LONG(dz, (int32_t)value);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
						break;
						case TYPE_SFIXED32:
						{
							int32_t l = 0;

							memcpy(&l, data, 4);
							MAKE_STD_ZVAL(dz);

							ZVAL_LONG(dz, (int32_t)l);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
							data += 4;
						}
						break;
						case TYPE_SFIXED64:
						{
							int64_t l;
							memcpy(&l, data, 8);

							MAKE_STD_ZVAL(dz);
							ZVAL_DOUBLE(dz, l);
							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);

							data += 8;
						}
						break;
						case TYPE_SINT32:
							data = ReadVarint32FromArray(data, &value, data_end);

							MAKE_STD_ZVAL(dz);
							ZVAL_LONG(dz, (int32_t)zigzag_decode32(value));

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
						break;
						case TYPE_SINT64:
						{
							uint64_t v2;
							data = ReadVarint64FromArray(data, &v2, data_end);

							MAKE_STD_ZVAL(dz);
							ZVAL_LONG(dz, (int64_t)zigzag_decode64(v2));

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
						}
						break;
					}

					if (last_data_offset == data) {
						php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_decode_message: detect infinite loop when processing repeated packed field.");
						break;
					}
					last_data_offset = data;
				} while(data < packed_data_end);

			}

			data += value;
		break;
		case WIRETYPE_FIXED32: {
			if (s == NULL) {
				if (container->process_unknown_fields > 0) {
					MAKE_STD_ZVAL(dz);
					process_unknown_field(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hresult, dz, tag, wiretype, value);
				} else {
					/* skip unknown field */
					zval_ptr_dtor(&dz);
				}
			} else if (s->type == TYPE_FLOAT) {
				uint32_t _v;
				float a = 0;

				memcpy(&_v, data, 4);
				a = decode_float(_v);

				MAKE_STD_ZVAL(dz);
				ZVAL_DOUBLE(dz, a);
			} else if (s->type == TYPE_SFIXED32) {
				int32_t l = 0;

				memcpy(&l, data, 4);
				MAKE_STD_ZVAL(dz);

				ZVAL_LONG(dz, (int32_t)l);
			} else if (s->type == TYPE_FIXED32) {
				uint32_t l = 0;
				memcpy(&l, data, 4);

				MAKE_STD_ZVAL(dz);
#if SIZEOF_LONG == 4
				if (l > 0x7fffffff) {
					/* PHP_INT_MAX is 0x7fffffff on this platform. cast to double. */
					ZVAL_DOUBLE(dz, l);
				} else {
					ZVAL_LONG(dz, (unsigned long)l);
				}
#else
				ZVAL_LONG(dz, (unsigned long)l);
#endif
			} else {
				int32_t l = 0;

				memcpy(&l, data, 4);

				MAKE_STD_ZVAL(dz);
				ZVAL_LONG(dz, l);
			}

			php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);

			data += 4;
		}
		break;
		}
	}

	return data;
}



void pb_serializer_destroy(pb_serializer *serializer)
{
	if (serializer != NULL) {
		if (serializer->buffer != NULL) {
			efree(serializer->buffer);
		}

		serializer->buffer = NULL;
		serializer->buffer_offset = 0;
		serializer->buffer_size = 0;
		efree(serializer);
		serializer = NULL;
	}
}


void pb_serializer_init(pb_serializer **serializer)
{
	pb_serializer *ser;
	ser = (pb_serializer*)emalloc(sizeof(pb_serializer));

	ser->buffer_size = 0;
	ser->buffer_capacity = 256;
	ser->buffer_offset = 0;
	ser->buffer = (uint8_t*)emalloc(sizeof(uint8_t) * ser->buffer_capacity);
	memset(ser->buffer, '\0', ser->buffer_capacity);

	*serializer = ser;
}

int pb_serializer_resize(pb_serializer *serializer, size_t size)
{
	if (serializer->buffer_size + size < serializer->buffer_capacity) {
		return 0;
	}

	while (serializer->buffer_size + size >= serializer->buffer_capacity) {
		serializer->buffer_capacity *= 2;
	}

	serializer->buffer = (uint8_t*)erealloc(serializer->buffer, serializer->buffer_capacity);
	if (serializer->buffer == NULL) {
		return 1;
	}

	return 0;
}

int pb_serializer_write32_le(pb_serializer *serializer, unsigned int value)
{
	uint8_t target[4] = {0};

	if (pb_serializer_resize(serializer, 1)) {
		return 1;
	}

#ifdef PROTOBUF_LITTLE_ENDIAN
	memcpy(target, (void*)&value, sizeof(value));
#else
	target[0] = (value);
	target[1] = (value >>  8);
	target[2] = (value >> 16);
	target[3] = (value >> 24);
#endif

	serializer->buffer[serializer->buffer_size++] = target[0];
	serializer->buffer[serializer->buffer_size++] = target[1];
	serializer->buffer[serializer->buffer_size++] = target[2];
	serializer->buffer[serializer->buffer_size++] = target[3];

	return 0;
}

int pb_serializer_write64_le(pb_serializer *serializer, uint64_t value)
{
	uint8_t target[8] = {0};

	if (pb_serializer_resize(serializer, 1)) {
		return 1;
	}

#ifdef PROTOBUF_LITTLE_ENDIAN
	memcpy(target, (void*)&value, sizeof(value));
#else
	{
		uint32_t part0 = (uint32_t)(value);
		uint32_t part1 = (uint32_t)(value >> 32);

		target[0] = (uint8_t)(part0);
		target[1] = (uint8_t)(part0 >> 8);
		target[2] = (uint8_t)(part0 >> 16);
		target[3] = (uint8_t)(part0 >> 24);
		target[4] = (uint8_t)(part1);
		target[5] = (uint8_t)(part1 >> 8);
		target[6] = (uint8_t)(part1 >> 16);
		target[7] = (uint8_t)(part1 >> 24);
	}
#endif

	serializer->buffer[serializer->buffer_size++] = target[0];
	serializer->buffer[serializer->buffer_size++] = target[1];
	serializer->buffer[serializer->buffer_size++] = target[2];
	serializer->buffer[serializer->buffer_size++] = target[3];
	serializer->buffer[serializer->buffer_size++] = target[4];
	serializer->buffer[serializer->buffer_size++] = target[5];
	serializer->buffer[serializer->buffer_size++] = target[6];
	serializer->buffer[serializer->buffer_size++] = target[7];

	return 0;
}

int pb_serializer_write_chararray(pb_serializer *serializer, unsigned char *string, size_t len)
{
	int i;

	if (pb_serializer_resize(serializer, len)) {
		return 1;
	}

	for (i = 0; i < len; i++) {
		serializer->buffer[serializer->buffer_size++] = string[i];
	}
	return 0;
}

int pb_serializer_write_varint32(pb_serializer *serializer, uint32_t value)
{
	uint8_t bytes[kMaxVarint32Bytes];
	int size = 0, i;

	if (value > kint32max) {
/* TODO:		fprintf(stderr, "out of bounds. write_varint32 expects %d\n", kint32max);
		return 0;
*/
	}

	if (pb_serializer_resize(serializer, 4)) {
		return 1;
	}

	while (value > 0x7F) {
		bytes[size++] = (value & 0x7F) | 0x80;
		value >>= 7;
	}
	bytes[size++] = value & 0x7F;

	for (i = 0; i < size; i++) {
		serializer->buffer[serializer->buffer_size++] = bytes[i];
	}

	return 0;
}

int pb_serializer_write_varint64(pb_serializer *serializer, uint64_t value)
{
	uint8_t bytes[kMaxVarintBytes];
	int size = 0, i;

	if (pb_serializer_resize(serializer, 8)) {
		return 1;
	}

	while (value > 0x7F) {
		bytes[size++] = ((value) & 0x7F) | 0x80;
		value >>= 7;
	}
	bytes[size++] = (value) & 0x7F;

	for (i = 0; i < size; i++) {
		serializer->buffer[serializer->buffer_size++] = bytes[i];
	}
	return 0;
}

void pb_encode_element_float(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_DOUBLE) {
		convert_to_double(*element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED32);
	}
	pb_serializer_write32_le(ser, encode_float(Z_DVAL_PP(element)));
}

void pb_encode_element_double(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_DOUBLE) {
		convert_to_double(*element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED64);
	}
	pb_serializer_write64_le(ser, encode_double(Z_DVAL_PP(element)));
}

void pb_encode_element_fixed32(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED32);
	}

	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}

	pb_serializer_write32_le(ser, Z_LVAL_PP(element));
}

void pb_encode_element_fixed64(PB_ENCODE_CALLBACK_PARAMETERS)
{
	long v;

	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}
	v = Z_LVAL_PP(element);

#if SIZEOF_LONG == 4
	if (v > 0x80000000 || v == 0x80000000) {
		zend_error(E_WARNING, "pb_encode_element_fixed64: 64bit long on 32bit platform. ignore this key");
		return;
	}
#endif

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED64);
	}

	if (v < 0) {
		pb_serializer_write64_le(ser, (int64_t)Z_LVAL_PP(element));
	} else {
		pb_serializer_write64_le(ser, (uint64_t)Z_LVAL_PP(element));
	}
}

void pb_encode_element_bool(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}
	pb_serializer_write_varint32(ser, Z_LVAL_PP(element));
}

void pb_encode_element_int64(PB_ENCODE_CALLBACK_PARAMETERS)
{
	long v;

	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}
	v = Z_LVAL_PP(element);

#if SIZEOF_LONG == 4
	if (v > 0x80000000 || v == 0x80000000) {
		zend_error(E_WARNING, "pb_encode_element_int64: 64bit long on 32bit platform. ignore this key");
		return;
	}
#endif

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}
	pb_serializer_write_varint64(ser, v);
}

void pb_encode_element_uint64(PB_ENCODE_CALLBACK_PARAMETERS)
{
	long v;

	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}
	v = Z_LVAL_PP(element);

#if SIZEOF_LONG == 4
	if (v > 0x80000000 || v == 0x80000000) {
		zend_error(E_WARNING, "pb_encode_element_int64: 64bit long on 32bit platform. ignore this key");
		return;
	}
#endif

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}
	pb_serializer_write_varint64(ser, v);
}

void pb_encode_element_int32(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}

	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}


	if (Z_LVAL_PP(element) < 0) {
		pb_serializer_write_varint64(ser, (uint64_t)Z_LVAL_PP(element));
	} else {
		pb_serializer_write_varint32(ser, Z_LVAL_PP(element));
	}
}

void pb_encode_element_string(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_NULL) {

		if (is_utf8(Z_STRVAL_PP(element), Z_STRLEN_PP(element)) < 1) {
			zend_throw_exception_ex(protocol_buffers_invalid_byte_sequence_class_entry, 0 TSRMLS_CC, "passed string is not valid utf8 string");
			return;
		}

		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_LENGTH_DELIMITED);
		pb_serializer_write_varint32(ser, Z_STRLEN_PP(element));

		pb_serializer_write_chararray(ser, (unsigned char*)Z_STRVAL_PP(element), Z_STRLEN_PP(element));
	}
}

void pb_encode_element_msg(PB_ENCODE_CALLBACK_PARAMETERS)
{
	zend_class_entry *ce;
	pb_scheme_container *n_container;
	pb_serializer *n_ser = NULL;
	int err = 0;

	ce = Z_OBJCE_PP(element);

	pb_get_scheme_container(ce->name, ce->name_length, &n_container, NULL TSRMLS_CC);
	if (err) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_get_scheme_cointainer failed. %s does not have getDescriptor method", ce->name);
	}
	/* TODO: add error handling */

	pb_encode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, *element, n_container, &n_ser);
	pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_LENGTH_DELIMITED);
	pb_serializer_write_varint32(ser, n_ser->buffer_size);
	pb_serializer_write_chararray(ser, n_ser->buffer, n_ser->buffer_size);

	efree(n_ser->buffer);
	efree(n_ser);
}

void pb_encode_element_bytes(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_STRLEN_PP(element) > 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_LENGTH_DELIMITED);
		pb_serializer_write_varint32(ser, Z_STRLEN_PP(element));
		pb_serializer_write_chararray(ser, (unsigned char*)Z_STRVAL_PP(element), Z_STRLEN_PP(element));
	}
}

void pb_encode_element_uint32(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}

	pb_serializer_write_varint32(ser, Z_LVAL_PP(element));
}

void pb_encode_element_enum(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}
	pb_serializer_write_varint32(ser, Z_LVAL_PP(element));
}

void pb_encode_element_sfixed32(PB_ENCODE_CALLBACK_PARAMETERS)
{
	long v;

	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}
	v = Z_LVAL_PP(element);

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED32);
	}
	pb_serializer_write32_le(ser, Z_LVAL_PP(element));
}

void pb_encode_element_sfixed64(PB_ENCODE_CALLBACK_PARAMETERS)
{
	uint64_t v;

	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}
	v = Z_LVAL_PP(element);

#if SIZEOF_LONG == 4
	if (v > 0x80000000 || v == 0x80000000) {
		zend_error(E_WARNING, "pb_encode_element_int64: 64bit long on 32bit platform. ignore this key");
		return;
	}
#endif

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED64);
	}
	pb_serializer_write64_le(ser, (uint64_t)v);
}

void pb_encode_element_sint32(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}
	pb_serializer_write_varint32(ser, zigzag_encode32(Z_LVAL_PP(element)));
}

void pb_encode_element_sint64(PB_ENCODE_CALLBACK_PARAMETERS)
{
	long v;

	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}
	v = Z_LVAL_PP(element);

#if SIZEOF_LONG == 4
	if (v > 0x80000000 || v == 0x80000000) {
		zend_error(E_WARNING, "pb_encode_element_sint64: 64bit long on 32bit platform. ignore this key");
		return;
	}
#endif

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}

	pb_serializer_write_varint64(ser, zigzag_encode64(v));
}

void pb_encode_element(INTERNAL_FUNCTION_PARAMETERS, pb_scheme_container *container, HashTable *hash, pb_scheme *scheme, pb_serializer *ser, pb_encode_callback f, int is_packed)
{
	zval **tmp;
	char *name;
	int name_len;

	if (container->use_single_property < 1) {
		name = scheme->mangled_name;
		name_len = scheme->mangled_name_len;
	} else {
		name = scheme->name;
		name_len = scheme->name_len;
	}

	if (zend_hash_find(hash, name, name_len, (void **)&tmp) == SUCCESS) {
		pb_serializer *n_ser = NULL;

		if (scheme->repeated) {
			HashPosition pos;
			zval **element;

			if (is_packed == 1) {
				pb_serializer_init(&n_ser);
			} else {
				n_ser = ser;
			}

			if (Z_TYPE_PP(tmp) != IS_ARRAY) {
				array_init(*tmp);
			}

			for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(tmp), &pos);
							zend_hash_get_current_data_ex(Z_ARRVAL_PP(tmp), (void **)&element, &pos) == SUCCESS;
							zend_hash_move_forward_ex(Z_ARRVAL_PP(tmp), &pos)
			) {
				f(INTERNAL_FUNCTION_PARAM_PASSTHRU, element, scheme, n_ser, is_packed);
			}

			if (is_packed == 1) {
				pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_LENGTH_DELIMITED);
				pb_serializer_write_varint32(ser, n_ser->buffer_size);
				pb_serializer_write_chararray(ser, (unsigned char*)n_ser->buffer, n_ser->buffer_size);

				pb_serializer_destroy(n_ser);
			}

		} else {
			if (is_packed == 1) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_encode_element_packed called non repeated scheme. this is bug");
			} else {
				if (scheme->required > 0 && Z_TYPE_PP(tmp) == IS_NULL) {
					zend_throw_exception_ex(protocol_buffers_invalid_protocolbuffers_exception_class_entry, 0 TSRMLS_CC, "the class does not have required property `%s`.", scheme->name);
					return;
				}
				f(INTERNAL_FUNCTION_PARAM_PASSTHRU, tmp, scheme, ser, is_packed);
			}
		}
	} else {
		if (scheme->required > 0) {
			zend_throw_exception_ex(protocol_buffers_invalid_protocolbuffers_exception_class_entry, 0 TSRMLS_CC, "the class does not declared required property `%s`. probably you missed declaration", scheme->name);
			return;
		}
	}
}

int pb_encode_message(INTERNAL_FUNCTION_PARAMETERS, zval *klass, pb_scheme_container *container, pb_serializer **serializer)
{
	int i = 0;
	pb_serializer *ser;
	zval **c;
	HashTable *hash = NULL;
	pb_scheme *scheme;

	pb_serializer_init(&ser);

	/* TODO: consider interface.
	pb_execute_sleep(klass, &targets TSRMLS_CC);
	if (targets != NULL) {
		TODO: make hash?
	}
	*/

	if (container->use_single_property < 1) {
		hash = Z_OBJPROP_P(klass);
	} else {
		if (zend_hash_find(Z_OBJPROP_P(klass), container->single_property_name, container->single_property_name_len, (void**)&c) == SUCCESS) {
			hash = Z_ARRVAL_PP(c);
		} else {
			pb_serializer_destroy(ser);

			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "the class does not have `_properties` protected property.");
			return -1;
		}
	}

	if (container->size < 1) {
		pb_serializer_destroy(ser);
		return -1;
	}

	for (i = 0; i < container->size; i++) {
		scheme = &(container->scheme[i]);

		switch (scheme->type) {
			case TYPE_DOUBLE:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_double, scheme->packed);
			break;
			case TYPE_FLOAT:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_float, scheme->packed);
			break;
			case TYPE_INT64:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_int64, scheme->packed);
			break;
			case TYPE_UINT64:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_uint64, scheme->packed);
			break;
			case TYPE_INT32:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_int32, scheme->packed);
			break;
			case TYPE_FIXED64:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_fixed64, scheme->packed);
			break;
			case TYPE_FIXED32:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_fixed32, scheme->packed);
			break;
			case TYPE_BOOL:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_bool, scheme->packed);
			break;
			case TYPE_STRING:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_string, 0);
			break;
			case TYPE_GROUP:
			break;
			case TYPE_MESSAGE:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_msg, 0);
			break;
			case TYPE_BYTES:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_bytes, 0);
			break;
			case TYPE_UINT32:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_uint32, scheme->packed);
			break;
			case TYPE_ENUM:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_enum, scheme->packed);
			break;
			case TYPE_SFIXED32:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_sfixed32, scheme->packed);
			break;
			case TYPE_SFIXED64:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_sfixed64, scheme->packed);
			break;
			case TYPE_SINT32:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_sint32, scheme->packed);
			break;
			case TYPE_SINT64:
				pb_encode_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hash, scheme, ser, &pb_encode_element_sint64, scheme->packed);
			break;
			default:
			break;
		}

		if (EG(exception)) {
			pb_serializer_destroy(ser);
			return 1;
		}
	}

	if (container->process_unknown_fields > 0) {
		char *uname;
		int uname_len;
		zval **unknown;

		if (container->use_single_property > 0) {
			uname = "_unknown";
			uname_len = sizeof("_unknown");
		} else {
			zend_mangle_property_name(&uname, &uname_len, (char*)"*", 1, (char*)"_unknown", sizeof("_unknown"), 0);
		}

		if (zend_hash_find(hash, uname, uname_len, (void**)&unknown) == SUCCESS) {
			if (Z_TYPE_PP(unknown) == IS_ARRAY) {
				HashTable *unkht;
				HashPosition pos;
				zval **element;

				unkht = Z_ARRVAL_PP(unknown);

				for(zend_hash_internal_pointer_reset_ex(unkht, &pos);
								zend_hash_get_current_data_ex(unkht, (void **)&element, &pos) == SUCCESS;
								zend_hash_move_forward_ex(unkht, &pos)
				) {
					//php_var_dump(element, 1 TSRMLS_CC);
				}

			}
		}

		if (container->use_single_property < 1) {
			efree(uname);
		}
	}


	*serializer = ser;
	return 0;
}

int php_protocolbuffers_encode(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *ce, zval *klass)
{
	int err;
	pb_serializer *ser = NULL;
	pb_scheme_container *container;

	err = pb_get_scheme_container(ce->name, ce->name_length, &container, NULL TSRMLS_CC);
	if (err) {
		if (EG(exception)) {
			return 1;
		} else {
			/* TODO: improve displaying error message */
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_get_scheme_cointainer failed. %s does not have getDescriptor method", ce->name);
			return 1;
		}
	}

	if (pb_encode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, klass, container, &ser)) {
		return 1;
	}

	if (ser == NULL) {
		return 1;
	}

	if (ser->buffer_size > 0) {
		RETVAL_STRINGL((char*)ser->buffer, ser->buffer_size, 1);

		pb_serializer_destroy(ser);
		return 0;
	} else {
		pb_serializer_destroy(ser);
		return 0;
	}
}

int php_protocolbuffers_decode(INTERNAL_FUNCTION_PARAMETERS, const char *data, int data_len, const char *klass, int klass_len)
{
	long buffer_size = 0;
	zval *obj = NULL;
	pb_scheme_container *container;
	const char *data_end, *res;
	int err = 0;

	if (data_len < 1) {
		zend_throw_exception_ex(protocol_buffers_invalid_protocolbuffers_exception_class_entry, 0 TSRMLS_CC, "passed variable seems null");
		return 1;
	}

	buffer_size = (long)data + sizeof(data);
	err = pb_get_scheme_container(klass, klass_len, &container, NULL TSRMLS_CC);
	if (err) {
		if (EG(exception)) {
			// do nothing
		} else {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_get_scheme_container failed. %s does not have getDescriptor method", klass);
		}
		return 1;
	}

	data_end = data + data_len;

	{
		zend_class_entry **ce = NULL;

		if (PBG(classes)) {
			/* Memo: fast lookup */
			if (zend_hash_find(PBG(classes), klass, klass_len, (void **)&ce) == FAILURE) {
				zend_lookup_class(klass, klass_len, &ce TSRMLS_CC);
				if (ce != NULL) {
					zend_hash_update(PBG(classes), klass, klass_len, (void **)ce, sizeof(ce), NULL);
				} else {
					php_error_docref(NULL TSRMLS_CC, E_ERROR, "class lookup failed. %s does exist", klass);
					return 1;
				}
			}
		}

		MAKE_STD_ZVAL(obj);
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
			return 0;
		}

		/* TODO:
		pb_execute_wakeup(obj TSRMLS_CC);
		*/
	}

	RETVAL_ZVAL(obj, 0, 1);
	return 0;
}
