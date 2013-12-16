#ifndef PHP_PROTOCOLBUFFERS_HELPER_H
#define PHP_PROTOCOLBUFFERS_HELPER_H

#ifdef _MSC_VER
#define strtoull _strtoui64
#define atoll _atoi64
#endif

typedef struct {
	int type;
	union {
		int32_t  int32;
		uint32_t uint32;
		int64_t  int64;
		uint64_t uint64;
		double d;
		float f;
	} value;
} pbf;

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

static inline int pb_get_lval_from_hash_by_tag(HashTable *proto, ulong tag, const char *name, size_t name_len TSRMLS_DC)
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

static inline int pb_get_zval_from_hash_by_tag(HashTable *proto, ulong tag, const char *name, size_t name_len, zval **result TSRMLS_DC)
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

void pb_execute_wakeup(zval *obj, pb_scheme_container *container TSRMLS_DC);

void pb_execute_sleep(zval *obj, pb_scheme_container *container TSRMLS_DC);

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

static inline pb_scheme *pb_search_scheme_by_tag(pb_scheme* scheme, uint scheme_size, uint tag)
{
	int i = 0;

	for (i = 0; i < scheme_size; i++) {
		if (scheme[i].tag == tag) {
			return &scheme[i];
		}
	}

	return NULL;
}

static inline void php_pb_decode_add_value_and_consider_repeated(pb_scheme_container *container, pb_scheme *s, HashTable *hresult, zval *dz TSRMLS_DC)
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



#define PB_ENCODE_CALLBACK_PARAMETERS INTERNAL_FUNCTION_PARAMETERS, zval **element, pb_scheme *scheme, pb_serializer *ser, int is_packed
typedef void (*pb_encode_callback)(PB_ENCODE_CALLBACK_PARAMETERS);

void pb_convert_msg(HashTable *proto, const char *klass, int klass_len, pb_scheme **scheme, int *size TSRMLS_DC);
int pb_get_scheme_container(const char *klass, size_t klass_len, pb_scheme_container **result, HashTable *descriptor TSRMLS_DC);
void process_unknown_field(INTERNAL_FUNCTION_PARAMETERS, pb_scheme_container *container, HashTable *hresult, zval *dz, int tag, int wiretype, int64_t value);
void process_unknown_field_bytes(INTERNAL_FUNCTION_PARAMETERS, pb_scheme_container *container, HashTable *hresult, int tag, int wiretype, uint8_t *bytes, int length);
const char* pb_decode_message(INTERNAL_FUNCTION_PARAMETERS, const char *data, const char *data_end, pb_scheme_container *container, zval **result);
void pb_serializer_destroy(pb_serializer *serializer);
void pb_serializer_init(pb_serializer **serializer);
int pb_serializer_resize(pb_serializer *serializer, size_t size);
int pb_serializer_write32_le(pb_serializer *serializer, unsigned int value);
int pb_serializer_write64_le(pb_serializer *serializer, uint64_t value);
int pb_serializer_write_chararray(pb_serializer *serializer, unsigned char *string, size_t len);
int pb_serializer_write_varint32(pb_serializer *serializer, uint32_t value);
int pb_serializer_write_varint64(pb_serializer *serializer, uint64_t value);
void pb_encode_element_float(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_double(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_fixed32(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_fixed64(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_bool(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_int64(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_uint64(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_int32(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_string(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_msg(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_bytes(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_uint32(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_enum(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_sfixed32(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_sfixed64(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_sint32(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element_sint64(PB_ENCODE_CALLBACK_PARAMETERS);
void pb_encode_element(INTERNAL_FUNCTION_PARAMETERS, pb_scheme_container *container, HashTable *hash, pb_scheme *scheme, pb_serializer *ser, pb_encode_callback f, int is_packed);
int pb_encode_message(INTERNAL_FUNCTION_PARAMETERS, zval *klass, pb_scheme_container *container, pb_serializer **serializer);

int php_protocolbuffers_encode(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *ce, zval *klass);
int php_protocolbuffers_decode(INTERNAL_FUNCTION_PARAMETERS, const char *data, int data_len, const char *klass, int klass_len);

void php_pb_helper_class(TSRMLS_D);
int php_pb_properties_init(zval *object, zend_class_entry *ce TSRMLS_DC);
char *pb_get_default_single_property_name();
int pb_get_default_single_property_name_len();

char *pb_get_default_unknown_property_name();
int pb_get_default_unknown_property_name_len();

void pb_scheme_container_init(pb_scheme_container *container);

void php_pb_helper_debug_zval(zval **value TSRMLS_DC);

void pb_format_string(zval *result, pbf *payload TSRMLS_DC);
#endif