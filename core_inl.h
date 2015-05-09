#ifndef PHP_PROTOCOLBUFFERS_HELPER_INL_H
#define PHP_PROTOCOLBUFFERS_HELPER_INL_H

static inline int is_utf8(const char *s, int len)
{
    int i;
    const unsigned char *bytes = (const unsigned char *)s;

    if (len < 1) {
      /* NOTE: always return 1 when passed string is null */
      return 1;
    }

    for (i = 0; i < len; i++) {
      /* ASCII */
      if (bytes[0] == 0x09 ||
          bytes[0] == 0x0A ||
          bytes[0] == 0x0D ||
          (bytes[0] >= 0x20 && bytes[0] <= 0x7E)
      ) {
        bytes += 1;
        continue;
      }

      /* non-overlong 2-byte */
      if (((i+1) <= len) &&
          (bytes[0] >= 0xC2 && bytes[0] <= 0xDF) &&
          (bytes[1] >= 0x80 && bytes[1] <= 0xBF)) {
        bytes += 2;
        i+=1;
        continue;
      }

      /* excluding overlongs */
      if (((i+2) <= len) &&
        bytes[0] == 0xE0 &&
        (bytes[1] >= 0xA0 && bytes[1] <= 0xBF) &&
        (bytes[2] >= 0x80 && bytes[2] <= 0xBF)
      ) {
        bytes += 3;
        i+=2;
        continue;
      }

      /* straight 3-byte */
      if (((i+2) <= len) &&
        ((bytes[0] >= 0xE1 && bytes[0] <= 0xEC) ||
          bytes[0] == 0xEE ||
          bytes[0] == 0xEF) &&
          (bytes[1] >= 0x80 && bytes[1] <= 0xBF) &&
          (bytes[2] >= 0x80 && bytes[2] <= 0xBF)
          ) {
            bytes += 3;
            i+=2;
            continue;
      }

      /* excluding surrogates */
      if (((i+2) <= len) &&
        bytes[0] == 0xED &&
        (bytes[1] >= 0x80 && bytes[1] <= 0x9F) &&
        (bytes[2] >= 0x80 && bytes[2] <= 0xBF)
      ) {
          bytes += 3;
          i+=2;
          continue;
      }

      /* planes 1-3 */
      if (((i+3) <= len) &&
        bytes[0] == 0xF0 &&
        (bytes[1] >= 0x90 && bytes[1] <= 0xBF) &&
        (bytes[2] >= 0x80 && bytes[2] <= 0xBF) &&
        (bytes[3] >= 0x80 && bytes[3] <= 0xBF)
      ) {
          bytes += 4;
          i+=3;
          continue;
      }

      /* planes 4-15 */
      if (((i+3) <= len) &&
        bytes[0] >= 0xF1 && bytes[0] <= 0xF3 &&
        bytes[1] >= 0x80 && bytes[1] <= 0xBF &&
        bytes[2] >= 0x80 && bytes[2] <= 0xBF &&
        bytes[3] >= 0x80 && bytes[3] <= 0xBF
      ) {
          bytes += 4;
          i+=3;
          continue;
      }

      /* plane 16 */
      if (((i+3) <= len) &&
        bytes[0] == 0xF4 &&
        (bytes[1] >= 0x80 && bytes[1] <= 0x8F) &&
        (bytes[2] >= 0x80 && bytes[2] <= 0xBF) &&
        (bytes[3] >= 0x80 && bytes[3] <= 0xBF)
      ) {
          bytes += 4;
          i+=3;
          continue;
      }

      return 0;
    }

    return 1;
}


static inline uint32_t zigzag_encode32(int32_t n) {
  // Note:  the right-shift must be arithmetic
  return (n << 1) ^ (n >> 31);
}

static inline int32_t zigzag_decode32(uint32_t n) {
  return (n >> 1) ^ - (int32_t)(n & 1);
}

static inline uint64_t zigzag_encode64(int64_t n) {
  // Note:  the right-shift must be arithmetic
  return (n << 1) ^ (n >> 63);
}

static inline int64_t zigzag_decode64(uint64_t n) {
  return (n >> 1) ^ - (int64_t)(n & 1);
}

static inline uint64_t encode_double(double value) {
    union {
        double d;
        uint64_t v;
    } u;
    u.d = value;

    return u.v;
}

static inline double decode_double(uint64_t value) {
    union {
        double d;
        uint64_t v;
    } u;
    u.v = value;

    return u.d;
}

static inline uint32_t encode_float(float value) {
    union {
        float f;
        uint32_t v;
    } u;
    u.f = value;

    return u.v;
}

static inline float decode_float(int32_t value) {
    union {
        float f;
        uint32_t v;
    } u;
    u.v = value;

    return u.f;
}

static inline int php_protocolbuffers_get_lval_from_hash_by_tag(HashTable *proto, ulong tag, const char *name, size_t name_len TSRMLS_DC)
{
    zval **d, **dd;

    if (zend_hash_index_find(proto, tag, (void **)&d) != SUCCESS) {
        return 0;
    }

    if (Z_TYPE_PP(d) != IS_ARRAY) {
        return 0;
    }

    if (zend_hash_find(Z_ARRVAL_PP(d), (char*)name, name_len, (void **)&dd) == SUCCESS) {
        return Z_LVAL_PP(dd);
    }

    return 0;
}

static inline int php_protocolbuffers_get_zval_from_hash_by_tag(HashTable *proto, ulong tag, const char *name, size_t name_len, zval **result TSRMLS_DC)
{
    zval **d, **dd;

    if (zend_hash_index_find(proto, tag, (void **)&d) != SUCCESS) {
        return 0;
    }

    if (Z_TYPE_PP(d) != IS_ARRAY) {
        return 0;
    }

    if (zend_hash_find(Z_ARRVAL_PP(d), (char*)name, name_len, (void **)&dd) == SUCCESS) {
        *result = *dd;
        return 1;
    }

    return 0;
}

static inline const char* ReadVarint32FromArray(const char* buffer, uint* value, const char* buffer_end) {
	/**
	 * Fast path:  We have enough bytes left in the buffer to guarantee that
	 * this read won't cross the end, so we can skip the checks.
	 */
	const char* ptr = buffer;
	int b;
	int result;
	int i;

	if (GOOGLE_PREDICT_TRUE(buffer < buffer_end) && (uint)*ptr < 0x80) {
		*value = (uint)*buffer;
		ptr++;
		return ptr;
	}

	b = *(ptr++); result  = (b & 0x7F)	  ; if (!(b & 0x80)) goto done;
	b = *(ptr++); result |= (b & 0x7F) <<  7; if (!(b & 0x80)) goto done;
	b = *(ptr++); result |= (b & 0x7F) << 14; if (!(b & 0x80)) goto done;
	b = *(ptr++); result |= (b & 0x7F) << 21; if (!(b & 0x80)) goto done;
	b = *(ptr++); result |=  b		 << 28; if (!(b & 0x80)) goto done;

	/**
	 * If the input is larger than 32 bits, we still need to read it all
	 * and discard the high-order bits.
	 */
	//for (i = 0; i < kMaxVarintBytes - kMaxVarint32Bytes; i++) {
	for (i = 0; i < 10 - 5; i++) {
		b = *(ptr++); if (!(b & 0x80)) goto done;
	}

	/**
	 * We have overrun the maximum size of a varint (10 bytes).  Assume
	 * the data is corrupt.
	 */
	return NULL;

	done:
		*value = result;
		return ptr;
}

static inline const char* ReadVarint64FromArray(const char* buffer, uint64_t* value, const char* buffer_end)
{
	const char* ptr = buffer;
	uint32_t b;

	/**
	 * Splitting into 32-bit pieces gives better performance on 32-bit
	 * processors.
	 */
	uint32_t part0 = 0, part1 = 0, part2 = 0;

	b = *(ptr++); part0  = (b & 0x7F)	  ; if (!(b & 0x80)) goto done;
	b = *(ptr++); part0 |= (b & 0x7F) <<  7; if (!(b & 0x80)) goto done;
	b = *(ptr++); part0 |= (b & 0x7F) << 14; if (!(b & 0x80)) goto done;
	b = *(ptr++); part0 |= (b & 0x7F) << 21; if (!(b & 0x80)) goto done;
	b = *(ptr++); part1  = (b & 0x7F)	  ; if (!(b & 0x80)) goto done;
	b = *(ptr++); part1 |= (b & 0x7F) <<  7; if (!(b & 0x80)) goto done;
	b = *(ptr++); part1 |= (b & 0x7F) << 14; if (!(b & 0x80)) goto done;
	b = *(ptr++); part1 |= (b & 0x7F) << 21; if (!(b & 0x80)) goto done;
	b = *(ptr++); part2  = (b & 0x7F)	  ; if (!(b & 0x80)) goto done;
	b = *(ptr++); part2 |= (b & 0x7F) <<  7; if (!(b & 0x80)) goto done;

	/**
	 * We have overrun the maximum size of a varint (10 bytes).  The data
	 * must be corrupt.
	 */
	return "";

   done:
	*value = (uint64_t)(part0)		 |
			 ((uint64_t)(part1) << 28) |
			 ((uint64_t)(part2) << 56);

	return ptr;
}

static inline php_protocolbuffers_scheme *php_protocolbuffers_search_scheme_by_tag(php_protocolbuffers_scheme* scheme, uint scheme_size, uint tag)
{
	int i = 0;

	for (i = 0; i < scheme_size; i++) {
		if (scheme[i].tag == tag) {
			return &scheme[i];
		}
	}

	return NULL;
}

static inline void php_protocolbuffers_decode_add_value_and_consider_repeated(php_protocolbuffers_scheme_container *container, php_protocolbuffers_scheme *s, HashTable *hresult, zval *dz TSRMLS_DC)
{
	char *name;
	int name_len;
	ulong hash;

	if (container->use_single_property < 1) {
		name     = s->mangled_name;
		name_len = s->mangled_name_len;
		hash     = s->mangled_name_h;
	} else {
		name     = s->name;
		name_len = s->name_len;
		hash     = s->name_h;
	}

	if (s->repeated) {
		if (!zend_hash_quick_exists(hresult, name, name_len, hash)) {
			zval *arr;

			MAKE_STD_ZVAL(arr);
			array_init(arr);

			zend_hash_next_index_insert(Z_ARRVAL_P(arr), (void *)&dz, sizeof(dz), NULL);
			Z_ADDREF_P(dz);

			zend_hash_quick_update(hresult, name, name_len, hash, (void **)&arr, sizeof(arr), NULL);
			Z_ADDREF_P(arr);
			zval_ptr_dtor(&arr);
		} else {
			zval **arr2;

			if (zend_hash_quick_find(hresult, name, name_len, hash, (void **)&arr2) == SUCCESS) {
				if (Z_TYPE_PP(arr2) == IS_NULL) {
					array_init(*arr2);
				}

				zend_hash_next_index_insert(Z_ARRVAL_PP(arr2), (void *)&dz, sizeof(dz), NULL);
				//Z_ADDREF_P(dz);
			}
		}
	} else {
		//Z_ADDREF_P(dz);
		zend_hash_quick_update(hresult, name, name_len, hash, (void **)&dz, sizeof(dz), NULL);
	}
}

static inline int php_protocolbuffers_process_varint(INTERNAL_FUNCTION_PARAMETERS, int wiretype, int tag, php_protocolbuffers_scheme_container *container, php_protocolbuffers_scheme *scheme, uint64_t value, HashTable *hresult)
{
	pbf __payload = {0};
	zval *dz = NULL;

	if (scheme == NULL) {
		if (container->process_unknown_fields > 0) {
			MAKE_STD_ZVAL(dz);
			php_protocolbuffers_process_unknown_field(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hresult, dz, tag, wiretype, value);
		} else {
			/* NOTE: skip unknown field. nothing to do. */
		}
	} else {
		MAKE_STD_ZVAL(dz);
		switch (scheme->type) {
			case TYPE_BOOL:
				ZVAL_BOOL(dz, value);
			break;
			case TYPE_INT32:
			case TYPE_ENUM:
				__payload.type = TYPE_INT32;__payload.value.int32 = (int32_t)value;
			break;
			case TYPE_UINT32:
				__payload.type = TYPE_UINT32;__payload.value.uint32 = (uint32_t)value;
			break;
			case TYPE_SINT32:
				__payload.type = TYPE_INT32;__payload.value.int32 = (int32_t)zigzag_decode32(value);
			break;
			case TYPE_INT64:
				__payload.type = TYPE_INT64;__payload.value.int64 = (int64_t)value;
			break;
			case TYPE_SINT64:
				__payload.type = TYPE_INT64;__payload.value.int64 = (int64_t)zigzag_decode64(value);
			break;
			case TYPE_UINT64:
				__payload.type = TYPE_UINT64;__payload.value.uint64 = (uint64_t)value;
			break;
			default:
				zval_ptr_dtor(&dz);
				return 0;
		}
		if (scheme->type != TYPE_BOOL) {
			php_protocolbuffers_format_string(dz, &__payload TSRMLS_CC);
		}

		php_protocolbuffers_decode_add_value_and_consider_repeated(container, scheme, hresult, dz TSRMLS_CC);
	}

	return 1;
}

static inline int php_protocolbuffers_process_fixed64(INTERNAL_FUNCTION_PARAMETERS, int wiretype, int tag, php_protocolbuffers_scheme_container *container, php_protocolbuffers_scheme *scheme, const char *data, HashTable *hresult)
{
	pbf __payload = {0};
	zval *dz = NULL;

	if (scheme == NULL) {
		if (container->process_unknown_fields > 0) {
			php_protocolbuffers_process_unknown_field_bytes(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hresult, tag, wiretype, (uint8_t *)data, 8);
		} else {
			/* skip unknown field */
		}
	} else {
		MAKE_STD_ZVAL(dz);

		switch (scheme->type) {
			case TYPE_DOUBLE:
			{
				uint64_t v;
				double d;

				memcpy(&v, data, 8);
				d = decode_double(v);

				__payload.type = TYPE_DOUBLE;__payload.value.d = d;
				php_protocolbuffers_format_string(dz, &__payload TSRMLS_CC);
			}
			break;
			case TYPE_FIXED64:
			{
				uint64_t v;

				memcpy(&v, data, 8);
				__payload.type = TYPE_UINT64;__payload.value.uint64 = v;
				php_protocolbuffers_format_string(dz, &__payload TSRMLS_CC);
			}
			break;
			case TYPE_SFIXED64:
			{
				int64_t l;
				memcpy(&l, data, 8);

				__payload.type = TYPE_INT64;__payload.value.int64 = l;
				php_protocolbuffers_format_string(dz, &__payload TSRMLS_CC);
			}
			break;
			default:
				zval_ptr_dtor(&dz);
				return 0;
		}
		php_protocolbuffers_decode_add_value_and_consider_repeated(container, scheme, hresult, dz TSRMLS_CC);
	}

	return 1;
}

static inline int php_protocolbuffers_process_fixed32(INTERNAL_FUNCTION_PARAMETERS, int wiretype, int tag, php_protocolbuffers_scheme_container *container, php_protocolbuffers_scheme *scheme, const char *data, HashTable *hresult)
{
	pbf __payload = {0};
	zval *dz = NULL;

	if (scheme == NULL) {
		if (container->process_unknown_fields > 0) {
			php_protocolbuffers_process_unknown_field_bytes(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hresult, tag, wiretype, (uint8_t*)data, 4);
		} else {
			/* skip unknown field */
		}
	} else {
		MAKE_STD_ZVAL(dz);

		switch(scheme->type) {
			case TYPE_FLOAT:
			{
				float a = 0;

				memcpy(&a, data, 4);
				__payload.type = TYPE_FLOAT;__payload.value.f = a;
			}
			break;
			case TYPE_SFIXED32:
			{
				int32_t l = 0;

				memcpy(&l, data, 4);
				__payload.type = TYPE_SINT32;__payload.value.int32 = l;
			}
			break;
			case TYPE_FIXED32:
			{
				uint32_t l = 0;
				memcpy(&l, data, 4);

				__payload.type = TYPE_UINT32;__payload.value.uint32 = l;
			}
			break;
			default:
				zval_ptr_dtor(&dz);
				return 0;
		}
		php_protocolbuffers_format_string(dz, &__payload TSRMLS_CC);
		php_protocolbuffers_decode_add_value_and_consider_repeated(container, scheme, hresult, dz TSRMLS_CC);
	}

	return 1;
}

#endif
