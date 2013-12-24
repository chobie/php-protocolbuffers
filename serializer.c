#include "protocolbuffers.h"
#include "serializer.h"

static int pb_serializer_resize(pb_serializer *serializer, size_t size);

static int pb_serializer_write32_le(pb_serializer *serializer, unsigned int value);

static int pb_serializer_write64_le(pb_serializer *serializer, uint64_t value);

static int pb_serializer_write_chararray(pb_serializer *serializer, unsigned char *string, size_t len);

static int pb_serializer_write_varint32(pb_serializer *serializer, uint32_t value);

static int pb_serializer_write_varint64(pb_serializer *serializer, uint64_t value);

static void pb_encode_element_float(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_double(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_fixed32(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_fixed64(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_bool(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_int64(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_uint64(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_int32(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_string(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_msg(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_bytes(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_uint32(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_enum(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_sfixed32(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_sfixed64(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_sint32(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element_sint64(PB_ENCODE_CALLBACK_PARAMETERS);

static void pb_encode_element(INTERNAL_FUNCTION_PARAMETERS, pb_scheme_container *container, HashTable *hash, pb_scheme *scheme, pb_serializer *ser, pb_encode_callback f, int is_packed);

static void php_protocolbuffers_encode_unknown_fields(pb_scheme_container *container, HashTable *hash, pb_serializer *ser TSRMLS_DC);

static int pb_serializer_resize(pb_serializer *serializer, size_t size)
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

static int pb_serializer_write32_le(pb_serializer *serializer, unsigned int value)
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

static int pb_serializer_write64_le(pb_serializer *serializer, uint64_t value)
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

static int pb_serializer_write_chararray(pb_serializer *serializer, unsigned char *string, size_t len)
{
	int i = 0;

	if (pb_serializer_resize(serializer, len)) {
		return 1;
	}

	for (i = 0; i < len; i++) {
		serializer->buffer[serializer->buffer_size++] = string[i];
	}
	return 0;
}

static int pb_serializer_write_varint32(pb_serializer *serializer, uint32_t value)
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

static int pb_serializer_write_varint64(pb_serializer *serializer, uint64_t value)
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

static void pb_encode_element_float(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_DOUBLE) {
		convert_to_double(*element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED32);
	}
	pb_serializer_write32_le(ser, encode_float(Z_DVAL_PP(element)));
}

static void pb_encode_element_double(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_DOUBLE) {
		convert_to_double(*element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED64);
	}
	pb_serializer_write64_le(ser, encode_double(Z_DVAL_PP(element)));
}

static void pb_encode_element_fixed32(PB_ENCODE_CALLBACK_PARAMETERS)
{
	uint32_t v = 0;

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED32);
	}

	if (Z_TYPE_PP(element) != IS_LONG) {
		if (Z_TYPE_PP(element) == IS_STRING) {
			v = (uint32_t)atol(Z_STRVAL_PP(element));
		} else {
			convert_to_long(*element);
			v = (uint32_t)Z_LVAL_PP(element);
		}
	} else {
		v = (uint32_t)Z_LVAL_PP(element);
	}

	pb_serializer_write32_le(ser, v);
}

static void pb_encode_element_fixed64(PB_ENCODE_CALLBACK_PARAMETERS)
{
	uint64_t v = 0;

	if (Z_TYPE_PP(element) != IS_LONG) {
		if (Z_TYPE_PP(element) == IS_STRING) {
			char *endptr = NULL;

			errno = 0;
			v = strtoull(Z_STRVAL_PP(element), &endptr, 0);
			if  (errno == ERANGE) {
				zend_error(E_WARNING, "pb_encode_element_fixed64: strtoull failed to decode string");
				return;
			}
		} else if (Z_TYPE_PP(element) == IS_DOUBLE) {
			v = (uint64_t)Z_DVAL_PP(element);
		} else {
			convert_to_long(*element);
			v = Z_LVAL_PP(element);
		}
	} else {
		v = Z_LVAL_PP(element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED64);
	}

	pb_serializer_write64_le(ser, (uint64_t)v);
}

static void pb_encode_element_bool(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}
	pb_serializer_write_varint32(ser, Z_LVAL_PP(element));
}

static void pb_encode_element_int64(PB_ENCODE_CALLBACK_PARAMETERS)
{
	int64_t v = 0;

	if (Z_TYPE_PP(element) != IS_LONG) {
		if (Z_TYPE_PP(element) == IS_STRING) {
			v = (int64_t)atoll(Z_STRVAL_PP(element));
		} else {
			convert_to_long(*element);
			v = (int64_t)Z_LVAL_PP(element);
		}
	} else {
		v = (int64_t)Z_LVAL_PP(element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}
	pb_serializer_write_varint64(ser, v);
}

static void pb_encode_element_uint64(PB_ENCODE_CALLBACK_PARAMETERS)
{
	uint64_t v = 0;

	if (Z_TYPE_PP(element) != IS_LONG) {
		if (Z_TYPE_PP(element) == IS_STRING) {
			char *bad;
			v = strtoull(Z_STRVAL_PP(element), &bad, 0);
		} else {
			convert_to_long(*element);
			v = (uint64_t)Z_LVAL_PP(element);
		}
	} else {
		v = (uint64_t)Z_LVAL_PP(element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}
	pb_serializer_write_varint64(ser, v);
}

static void pb_encode_element_int32(PB_ENCODE_CALLBACK_PARAMETERS)
{
	int32_t v = 0;

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}

	if (Z_TYPE_PP(element) != IS_LONG) {
		if (Z_TYPE_PP(element) == IS_STRING) {
			v = (int32_t)atol(Z_STRVAL_PP(element));
		} else {
			convert_to_long(*element);
			v = (int32_t)Z_LVAL_PP(element);
		}
	} else {
		v = (int32_t)Z_LVAL_PP(element);
	}

	pb_serializer_write_varint32(ser,(uint32_t)v);
}

static void pb_encode_element_string(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_NULL) {

		if (PBG(validate_string) && is_utf8(Z_STRVAL_PP(element), Z_STRLEN_PP(element)) < 1) {
			zend_throw_exception_ex(php_protocol_buffers_invalid_byte_sequence_class_entry, 0 TSRMLS_CC, "passed string is not valid utf8 string");
			return;
		}

		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_LENGTH_DELIMITED);
		pb_serializer_write_varint32(ser, Z_STRLEN_PP(element));

		pb_serializer_write_chararray(ser, (unsigned char*)Z_STRVAL_PP(element), Z_STRLEN_PP(element));
	}
}

static void pb_encode_element_msg(PB_ENCODE_CALLBACK_PARAMETERS)
{
	zend_class_entry *ce = NULL;
	pb_scheme_container *n_container = NULL;
	pb_serializer *n_ser = NULL;
	int err = 0;

	ce = Z_OBJCE_PP(element);

	php_protocolbuffers_get_scheme_container(ce->name, ce->name_length, &n_container TSRMLS_CC);
	if (err) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "php_protocolbuffers_get_scheme_container failed. %s does not have getDescriptor method", ce->name);
		return;
	}
	/* TODO: add error handling */

	php_protocolbuffers_encode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, *element, n_container, &n_ser);
	if (EG(exception)) {
		return;
	}

	pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_LENGTH_DELIMITED);
	pb_serializer_write_varint32(ser, n_ser->buffer_size);
	pb_serializer_write_chararray(ser, n_ser->buffer, n_ser->buffer_size);

	php_protocolbuffers_serializer_destroy(n_ser);
}

static void pb_encode_element_bytes(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_STRLEN_PP(element) > 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_LENGTH_DELIMITED);
		pb_serializer_write_varint32(ser, Z_STRLEN_PP(element));
		pb_serializer_write_chararray(ser, (unsigned char*)Z_STRVAL_PP(element), Z_STRLEN_PP(element));
	}
}

static void pb_encode_element_uint32(PB_ENCODE_CALLBACK_PARAMETERS)
{
	uint32_t v = 0;

	if (Z_TYPE_PP(element) != IS_LONG) {
		if (Z_TYPE_PP(element) == IS_STRING) {
			v = (int32_t)atol(Z_STRVAL_PP(element));
		} else {
			convert_to_long(*element);
			v = (int32_t)Z_LVAL_PP(element);
		}
	} else {
		v = (int32_t)Z_LVAL_PP(element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}

	pb_serializer_write_varint32(ser, v);
}

static void pb_encode_element_enum(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}
	pb_serializer_write_varint32(ser, Z_LVAL_PP(element));
}

static void pb_encode_element_sfixed32(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED32);
	}
	pb_serializer_write32_le(ser, Z_LVAL_PP(element));
}

static void pb_encode_element_sfixed64(PB_ENCODE_CALLBACK_PARAMETERS)
{
	int64_t v = 0;

	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}
	v = Z_LVAL_PP(element);

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED64);
	}
	pb_serializer_write64_le(ser, (int64_t)v);
}

static void pb_encode_element_sint32(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}
	pb_serializer_write_varint32(ser, zigzag_encode32(Z_LVAL_PP(element)));
}

static void pb_encode_element_sint64(PB_ENCODE_CALLBACK_PARAMETERS)
{
	int64_t v = 0;

	if (Z_TYPE_PP(element) != IS_LONG) {
		if (Z_TYPE_PP(element) == IS_STRING) {
			v = (int64_t)atoll(Z_STRVAL_PP(element));
		} else {
			convert_to_long(*element);
			v = (int64_t)Z_LVAL_PP(element);
		}
	} else {
		v = (int64_t)Z_LVAL_PP(element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
	}

	pb_serializer_write_varint64(ser, zigzag_encode64(v));
}

static void pb_encode_element(INTERNAL_FUNCTION_PARAMETERS, pb_scheme_container *container, HashTable *hash, pb_scheme *scheme, pb_serializer *ser, pb_encode_callback f, int is_packed)
{
	zval **tmp = NULL;
	char *name = {0};
	int name_len = 0;

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
				php_protocolbuffers_serializer_init(&n_ser);
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
				if (Z_TYPE_PP(element) == IS_NULL) {
					continue;
				}

				f(INTERNAL_FUNCTION_PARAM_PASSTHRU, element, scheme, n_ser, is_packed);
			}

			if (is_packed == 1) {
				pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_LENGTH_DELIMITED);
				pb_serializer_write_varint32(ser, n_ser->buffer_size);
				pb_serializer_write_chararray(ser, (unsigned char*)n_ser->buffer, n_ser->buffer_size);

				php_protocolbuffers_serializer_destroy(n_ser);
			}

		} else {
			if (is_packed == 1) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_encode_element_packed called non repeated scheme. this is bug");
			} else {
				if (scheme->required > 0 && Z_TYPE_PP(tmp) == IS_NULL) {
					zend_throw_exception_ex(php_protocol_buffers_uninitialized_message_exception_class_entry, 0 TSRMLS_CC, "the class does not have required property `%s`.", scheme->name);
					return;
				}
				if (scheme->required == 0 && Z_TYPE_PP(tmp) == IS_NULL) {
					return;
				}
				if (Z_TYPE_PP(tmp) == IS_ARRAY) {
					//php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s is not repeated field but array given", scheme->name);
					return;
				}

				f(INTERNAL_FUNCTION_PARAM_PASSTHRU, tmp, scheme, ser, is_packed);
			}
		}
	} else {
		if (scheme->required > 0) {
			zend_throw_exception_ex(php_protocol_buffers_invalid_protocolbuffers_exception_class_entry, 0 TSRMLS_CC, "the class does not declared required property `%s`. probably you missed declaration", scheme->name);
			return;
		}
	}
}

void php_protocolbuffers_serializer_destroy(pb_serializer *serializer)
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

void php_protocolbuffers_serializer_init(pb_serializer **serializer)
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

static void php_protocolbuffers_encode_unknown_fields(pb_scheme_container *container, HashTable *hash, pb_serializer *ser TSRMLS_DC)
{
	char *uname = {0};
	int uname_len = 0;
	zval **unknown = NULL;

	if (container->use_single_property > 0) {
		uname = "_unknown";
		uname_len = sizeof("_unknown");
	} else {
		zend_mangle_property_name(&uname, &uname_len, (char*)"*", 1, (char*)"_unknown", sizeof("_unknown"), 0);
	}

	if (zend_hash_find(hash, uname, uname_len, (void**)&unknown) == SUCCESS) {
		if (Z_TYPE_PP(unknown) == IS_OBJECT
		 && Z_OBJCE_PP(unknown) == php_protocol_buffers_unknown_field_set_class_entry) {
			HashTable *unkht;
			zval **element, **elmh;
			char *uuname;
			int uuname_len;

			unkht = Z_OBJPROP_PP(unknown);

			zend_mangle_property_name(&uuname, &uuname_len, (char*)"*", 1, (char*)"fields", sizeof("fields"), 0);
			if (zend_hash_find(unkht, uuname, uuname_len, (void**)&elmh) == SUCCESS) {
				HashPosition pos2;

				for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(elmh), &pos2);
								zend_hash_get_current_data_ex(Z_ARRVAL_PP(elmh), (void **)&element, &pos2) == SUCCESS;
								zend_hash_move_forward_ex(Z_ARRVAL_PP(elmh), &pos2)
				) {
					HashPosition pos;
					php_protocolbuffers_unknown_field *field = NULL;
					unknown_value **unknown;
					field = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, *element);

					switch (field->type) {
						case WIRETYPE_VARINT:
						{
							for(zend_hash_internal_pointer_reset_ex(field->ht, &pos);
												zend_hash_get_current_data_ex(field->ht, (void **)&unknown, &pos) == SUCCESS;
												zend_hash_move_forward_ex(field->ht, &pos)
								) {
								pb_serializer_write_varint32(ser, (field->number << 3) | field->type);
								pb_serializer_write_varint32(ser, (*unknown)->varint);
							}
						}
						break;
						case WIRETYPE_FIXED64:
							for(zend_hash_internal_pointer_reset_ex(field->ht, &pos);
												zend_hash_get_current_data_ex(field->ht, (void **)&unknown, &pos) == SUCCESS;
												zend_hash_move_forward_ex(field->ht, &pos)
								) {
								pb_serializer_write_varint32(ser, (field->number << 3) | field->type);
								pb_serializer_write_chararray(ser, (*unknown)->buffer.val, (*unknown)->buffer.len);
							}
						break;
						case WIRETYPE_LENGTH_DELIMITED:
						{
							for(zend_hash_internal_pointer_reset_ex(field->ht, &pos);
												zend_hash_get_current_data_ex(field->ht, (void **)&unknown, &pos) == SUCCESS;
												zend_hash_move_forward_ex(field->ht, &pos)
								) {
								pb_serializer_write_varint32(ser, (field->number << 3) | field->type);
								pb_serializer_write_varint32(ser, (*unknown)->buffer.len);
								pb_serializer_write_chararray(ser, (*unknown)->buffer.val, (*unknown)->buffer.len);
							}
						}
						break;
						case WIRETYPE_START_GROUP:
						break;
						case WIRETYPE_END_GROUP:
						break;
						case WIRETYPE_FIXED32:
							for(zend_hash_internal_pointer_reset_ex(field->ht, &pos);
												zend_hash_get_current_data_ex(field->ht, (void **)&unknown, &pos) == SUCCESS;
												zend_hash_move_forward_ex(field->ht, &pos)
								) {
								pb_serializer_write_varint32(ser, (field->number << 3) | field->type);
								pb_serializer_write_chararray(ser, (*unknown)->buffer.val, (*unknown)->buffer.len);
							}
						break;
					}
				}
			}
			efree(uuname);
		}
	}

	if (container->use_single_property < 1) {
		efree(uname);
	}
}

int php_protocolbuffers_encode_message(INTERNAL_FUNCTION_PARAMETERS, zval *klass, pb_scheme_container *container, pb_serializer **serializer)
{
	int i = 0;
	pb_serializer *ser;
	zval **c = NULL;
	HashTable *hash = NULL;
	pb_scheme *scheme;

	php_protocolbuffers_serializer_init(&ser);

	if (container->use_wakeup_and_sleep > 0) {
		php_protocolbuffers_execute_sleep(klass, container TSRMLS_CC);
	}

	if (container->use_single_property < 1) {
		hash = Z_OBJPROP_P(klass);
	} else {
		if (zend_hash_find(Z_OBJPROP_P(klass), container->single_property_name, container->single_property_name_len+1, (void**)&c) == SUCCESS) {
			hash = Z_ARRVAL_PP(c);
		} else {
			php_protocolbuffers_serializer_destroy(ser);
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "the class does not have `_properties` protected property.");
			return -1;
		}
	}

	if (container->size < 1 && container->process_unknown_fields < 1) {
		php_protocolbuffers_serializer_destroy(ser);
		return -1;
	}

	for (i = 0; i < container->size; i++) {
		scheme = &(container->scheme[i]);

		if (container->use_wakeup_and_sleep > 0) {
			if (scheme->skip > 0) {
				continue;
			}
		}

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
			php_protocolbuffers_serializer_destroy(ser);
			return 1;
		}
	}

	if (container->process_unknown_fields > 0) {
		php_protocolbuffers_encode_unknown_fields(container, hash, ser TSRMLS_CC);
	}

	*serializer = ser;
	return 0;
}
