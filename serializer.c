#include "php_protocolbuffers.h"
#include "serializer.h"

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
	int i = 0;

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

void pb_encode_element_fixed64(PB_ENCODE_CALLBACK_PARAMETERS)
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

void pb_encode_element_uint64(PB_ENCODE_CALLBACK_PARAMETERS)
{
	uint64_t v = 0;

	if (Z_TYPE_PP(element) != IS_LONG) {
		if (Z_TYPE_PP(element) == IS_STRING) {
			char *bad;
			// TODO: check portability
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

void pb_encode_element_int32(PB_ENCODE_CALLBACK_PARAMETERS)
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

void pb_encode_element_string(PB_ENCODE_CALLBACK_PARAMETERS)
{
	if (Z_TYPE_PP(element) != IS_NULL) {

		if (PBG(validate_string) && is_utf8(Z_STRVAL_PP(element), Z_STRLEN_PP(element)) < 1) {
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
	zend_class_entry *ce = NULL;
	pb_scheme_container *n_container = NULL;
	pb_serializer *n_ser = NULL;
	int err = 0;

	ce = Z_OBJCE_PP(element);

	pb_get_scheme_container(ce->name, ce->name_length, &n_container, NULL TSRMLS_CC);
	if (err) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_get_scheme_cointainer failed. %s does not have getDescriptor method", ce->name);
		return;
	}
	/* TODO: add error handling */

	pb_encode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, *element, n_container, &n_ser);
	if (EG(exception)) {
		return;
	}

	pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_LENGTH_DELIMITED);
	pb_serializer_write_varint32(ser, n_ser->buffer_size);
	pb_serializer_write_chararray(ser, n_ser->buffer, n_ser->buffer_size);

	pb_serializer_destroy(n_ser);
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
	if (Z_TYPE_PP(element) != IS_LONG) {
		convert_to_long(*element);
	}

	if (is_packed == 0) {
		pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED32);
	}
	pb_serializer_write32_le(ser, Z_LVAL_PP(element));
}

void pb_encode_element_sfixed64(PB_ENCODE_CALLBACK_PARAMETERS)
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

void pb_encode_element(INTERNAL_FUNCTION_PARAMETERS, pb_scheme_container *container, HashTable *hash, pb_scheme *scheme, pb_serializer *ser, pb_encode_callback f, int is_packed)
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
				if (Z_TYPE_PP(element) == IS_NULL) {
					continue;
				}

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
					zend_throw_exception_ex(protocol_buffers_uninitialized_message_exception_class_entry, 0 TSRMLS_CC, "the class does not have required property `%s`.", scheme->name);
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
			zend_throw_exception_ex(protocol_buffers_invalid_protocolbuffers_exception_class_entry, 0 TSRMLS_CC, "the class does not declared required property `%s`. probably you missed declaration", scheme->name);
			return;
		}
	}
}