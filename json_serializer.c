#include "protocolbuffers.h"
#include "serializer.h"
#include "json_serializer.h"

typedef struct {
	int (*serialize_double)(double value, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_float)(float value, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_int64)(int64_t value, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_uint64)(uint64_t value, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_int32)(int32_t value, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_fixed64)(uint64_t value, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_fixed32)(int32_t value, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_bool)(zend_bool value, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_string)(const char *value, size_t value_len, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_group)(void);
	int (*serialize_bytes)(void *value, size_t value_len, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_uint32)(uint32_t value, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_enum)(int32_t value, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_sfixed32)(int32_t value, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_sfixed64)(int64_t value, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_sint32)(int32_t value, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_sint64)(int64_t value, php_protocolbuffers_scheme *scheme, php_protocolbuffers_scheme_container *container, void *opaque TSRMLS_DC);
	int (*serialize_message)(zval *value,
		php_protocolbuffers_scheme *scheme,
		php_protocolbuffers_scheme_container *child_container,
		php_protocolbuffers_scheme_container *container,
		void *opaque TSRMLS_DC);
	int (*serialize_repeated_begin)(
		php_protocolbuffers_scheme *scheme,
		php_protocolbuffers_scheme_container *container,
		void *opaque TSRMLS_DC);
	int (*serialize_repeated_end)(zval *value,
		php_protocolbuffers_scheme *scheme,
		php_protocolbuffers_scheme_container *container,
		void *opaque TSRMLS_DC);
} php_protocolbuffers_serializer2;

static int _json_serializer_float(
	float value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_double(result, value);
	} else {
		add_assoc_double_ex(result, scheme->original_name, scheme->original_name_len, value);
	}

	return 0;
}

static int _json_serializer_double(
	double value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_double(result, value);
	} else {
		add_assoc_double_ex(result, scheme->original_name, scheme->original_name_len, value);
	}

	return 0;
}

static int _json_serializer_repeated_begin(
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval **result = (zval**)opaque;
	zval *outer = NULL;

	MAKE_STD_ZVAL(outer);
	array_init(outer);

	*result = outer;
	return 0;
}

static int _json_serializer_repeated_end(
	zval *value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *outer = (zval*)opaque;

	add_assoc_zval_ex(value, scheme->original_name, scheme->original_name_len, outer);

	return 0;
}

static int _json_serializer_enum(
	int32_t value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_long(result, value);
	} else {
		add_assoc_long_ex(result, scheme->original_name, scheme->original_name_len, value);
	}

	return 0;
}

static int _json_serializer_sint32(
	int32_t value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_long(result, value);
	} else {
		add_assoc_long_ex(result, scheme->original_name, scheme->original_name_len, value);
	}

	return 0;
}

static int _json_serializer_sint64(
	int64_t value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_long(result, value);
	} else {
		add_assoc_long_ex(result, scheme->original_name, scheme->original_name_len, value);
	}

	return 0;
}

static int _json_serializer_fixed64(
	uint64_t value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_long(result, value);
	} else {
		add_assoc_long_ex(result, scheme->original_name, scheme->original_name_len, value);
	}

	return 0;
}

static int _json_serializer_sfixed32(
	int32_t value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_long(result, value);
	} else {
		add_assoc_long_ex(result, scheme->original_name, scheme->original_name_len, value);
	}

	return 0;
}

static int _json_serializer_sfixed64(
	int64_t value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_long(result, value);
	} else {
		add_assoc_long_ex(result, scheme->original_name, scheme->original_name_len, value);
	}

	return 0;
}

static int _json_serializer_fixed32(
	int32_t value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_long(result, value);
	} else {
		add_assoc_long_ex(result, scheme->original_name, scheme->original_name_len, value);
	}

	return 0;
}

static int _json_serializer_uint32(
	uint32_t value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_long(result, value);
	} else {
		add_assoc_long_ex(result, scheme->original_name, scheme->original_name_len, value);
	}

	return 0;
}

static int _json_serializer_bool(
	zend_bool value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_bool(result, value);
	} else {
		add_assoc_bool_ex(result, scheme->original_name, scheme->original_name_len, value);
	}

	return 0;
}


static int _json_serializer_int64(
	int64_t value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_long(result, value);
	} else {
		add_assoc_long_ex(result, scheme->original_name, scheme->original_name_len, value);
	}

	return 0;
}

static int _json_serializer_uint64(
	uint64_t value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_long(result, value);
	} else {
		add_assoc_long_ex(result, scheme->original_name, scheme->original_name_len, value);
	}

	return 0;
}


static int _json_serializer_int32(
	int32_t value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC
)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_long(result, value);
	} else {
		add_assoc_long_ex(result, scheme->original_name, scheme->original_name_len, value);
	}

	return 0;
}

static int _json_serializer_bytes(
	void *value,
	size_t value_len,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_stringl(result, value, value_len, 1);
	} else {
		add_assoc_stringl_ex(result, scheme->original_name, scheme->original_name_len, value, value_len, 1);
	}

	return 0;
}


static int _json_serializer_string(
	const char *value,
	size_t value_len,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC)
{
	zval *result = (zval*)opaque;

	if (scheme->repeated) {
		add_next_index_stringl(result, value, value_len, 1);
	} else {
		add_assoc_stringl_ex(result, scheme->original_name, scheme->original_name_len, (char*)value, value_len, 1);
	}

	return 0;
}

static int _json_serializer_message(
	zval *value,
	php_protocolbuffers_scheme *scheme,
	php_protocolbuffers_scheme_container *child_container,
	php_protocolbuffers_scheme_container *container,
	void *opaque TSRMLS_DC) {

	zval *result = (zval*)opaque;
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	array_init(tmp);

	php_protocolbuffers_encode_jsonserialize(value, child_container, 0,  &tmp TSRMLS_CC);
	if (scheme->repeated) {
		add_next_index_zval(result, tmp);
	} else {
		add_assoc_zval_ex(result, scheme->original_name, scheme->original_name_len, tmp);
	}

	return 0;
}

static php_protocolbuffers_serializer2 json_serializer = {
	_json_serializer_double,//double
	_json_serializer_float,//float
	_json_serializer_int64,//int64
	_json_serializer_uint64,//uint64
	_json_serializer_int32,//int32
	_json_serializer_fixed64,//fixed64
	_json_serializer_fixed32,//fixed32
	_json_serializer_bool,//bool
	_json_serializer_string,//string
	NULL,//group
	_json_serializer_bytes,//bytes
	_json_serializer_uint32,//uint32
	_json_serializer_enum,//enum
	_json_serializer_sfixed32,//sfixed32
	_json_serializer_sfixed64,//sfixed64
	_json_serializer_sint32,//sint32
	_json_serializer_sint64,//sint64
	_json_serializer_message,//message
	_json_serializer_repeated_begin, //repeated_begin
	_json_serializer_repeated_end, //repeated_end
};


static const char* php_protocolbuffers_get_property_name(php_protocolbuffers_scheme_container *container, php_protocolbuffers_scheme *scheme, int *name_len)
{
	const char *name;
	if (container->use_single_property < 1) {
		name = scheme->mangled_name;
		*name_len = scheme->mangled_name_len;
	} else {
		name = scheme->name;
		*name_len = scheme->name_len;
	}

	return name;
}


static int convert_to_int64(zval *value, int64_t *result)
{
	int64_t v = 0;

	if (Z_TYPE_P(value) != IS_LONG) {
		if (Z_TYPE_P(value) == IS_STRING) {
			v = (int64_t)atoll(Z_STRVAL_P(value));
		} else {
			convert_to_long(value);
			v = (int64_t)Z_LVAL_P(value);
		}
	} else {
		v = (int64_t)Z_LVAL_P(value);
	}
	*result = v;

	return 0;
}

static int convert_to_uint64(zval *value, uint64_t *result)
{
	uint64_t v = 0;

	if (Z_TYPE_P(value) != IS_LONG) {
		if (Z_TYPE_P(value) == IS_STRING) {
			char *endptr = NULL;
			errno = 0;
			v = strtoull(Z_STRVAL_P(value), &endptr, 0);

			if  (errno == ERANGE) {
				zend_error(E_WARNING, "php_protocolbuffers_encode_value_fixed64: strtoull failed to decode string");
				return -1;
			}
		} else if (Z_TYPE_P(value) == IS_DOUBLE) {
			v = (uint64_t)Z_DVAL_P(value);
		} else {
			convert_to_long(value);
			v = Z_LVAL_P(value);
		}
	} else {
		v = Z_LVAL_P(value);
	}
	*result = v;

	return 0;
}

static int convert_to_uint32(zval *value, uint32_t *result)
{
	uint32_t v = 0;

	if (Z_TYPE_P(value) != IS_LONG) {
		if (Z_TYPE_P(value) == IS_STRING) {
			v = (uint32_t)atol(Z_STRVAL_P(value));
		} else {
			convert_to_long(value);
			v = (uint32_t)Z_LVAL_P(value);
		}
	} else {
		v = (uint32_t)Z_LVAL_P(value);
	}
	*result = v;

	return 0;
}

static int convert_to_int32(zval *value, int32_t *result)
{
	int32_t v = 0;

	if (Z_TYPE_P(value) != IS_LONG) {
		if (Z_TYPE_P(value) == IS_STRING) {
			v = (int32_t)atol(Z_STRVAL_P(value));
		} else {
			convert_to_long(value);
			v = (int32_t)Z_LVAL_P(value);
		}
	} else {
		v = (int32_t)Z_LVAL_P(value);
	}

	*result = v;
	return 0;
}

static int php_protocolbuffers_json_encode_value(zval **element, php_protocolbuffers_scheme_container *container, php_protocolbuffers_scheme *scheme, php_protocolbuffers_serializer2 *ser, int throws_exception, void *opaque TSRMLS_DC)
{
	zval value_copy;
	zval *outer = (zval*)opaque;

	value_copy = **element;
	switch (scheme->type) {
		case TYPE_DOUBLE:
			zval_copy_ctor(&value_copy);
			convert_to_double(&value_copy);

			ser->serialize_double((double)Z_DVAL_P(&value_copy), scheme, container, outer TSRMLS_CC);
			zval_dtor(&value_copy);
			break;
		case TYPE_FLOAT:
			zval_copy_ctor(&value_copy);
			convert_to_double(&value_copy);

			ser->serialize_float((float)Z_DVAL_P(&value_copy), scheme, container, outer TSRMLS_CC);
			zval_dtor(&value_copy);
			break;
		case TYPE_INT64:
		{
			int64_t v;

			zval_copy_ctor(&value_copy);
			convert_to_int64(&value_copy, &v);

			ser->serialize_int64(v, scheme, container, outer TSRMLS_CC);
			zval_dtor(&value_copy);
			break;
		}
		case TYPE_UINT64:
		{
			uint64_t v;

			zval_copy_ctor(&value_copy);
			convert_to_uint64(&value_copy, &v);

			ser->serialize_uint64(v, scheme, container, outer TSRMLS_CC);
			zval_dtor(&value_copy);
			break;
		}
		case TYPE_INT32:
		{
			int32_t v;

			zval_copy_ctor(&value_copy);
			convert_to_int32(&value_copy, &v);
			ser->serialize_int32(v, scheme, container, outer TSRMLS_CC);
			zval_dtor(&value_copy);
			break;
		}
		case TYPE_FIXED64:
		{
			uint64_t v;

			zval_copy_ctor(&value_copy);
			convert_to_uint64(&value_copy, &v);
			ser->serialize_fixed64(v, scheme, container, outer TSRMLS_CC);
			zval_dtor(&value_copy);
			break;
		}
		case TYPE_FIXED32:
		{
			uint32_t v;

			zval_copy_ctor(&value_copy);
			convert_to_uint32(&value_copy, &v);
			ser->serialize_fixed32(v, scheme, container, outer TSRMLS_CC);
			zval_dtor(&value_copy);
			break;

		}
		case TYPE_BOOL:
			zval_copy_ctor(&value_copy);
			convert_to_boolean(&value_copy);

			ser->serialize_bool(Z_BVAL_P(&value_copy), scheme, container, outer TSRMLS_CC);
			zval_dtor(&value_copy);
			break;
		case TYPE_STRING:
			if (Z_STRLEN_PP(element) == 0 || (Z_STRLEN_PP(element) == 1 && Z_STRVAL_PP(element)[0] == '0')) {
				return -1;
			}

			if (PBG(validate_string) && is_utf8(Z_STRVAL_PP(element), Z_STRLEN_PP(element)) < 1) {
				php_protocolbuffers_raise_error_or_exception(php_protocol_buffers_invalid_byte_sequence_class_entry, E_WARNING, throws_exception, "passed string is not valid utf8 string");
				return -1;
			}

			ser->serialize_string(Z_STRVAL_PP(element), Z_STRLEN_PP(element), scheme, container, outer TSRMLS_CC);
			break;
		case TYPE_GROUP:
			break;
		case TYPE_MESSAGE: {
			php_protocolbuffers_scheme_container *child_scheme;
			php_protocolbuffers_get_scheme_container(scheme->ce->name, scheme->ce->name_length, &child_scheme TSRMLS_CC);

			ser->serialize_message(*element, scheme, child_scheme, container, outer TSRMLS_CC);
			break;
		}
		case TYPE_BYTES:
			if (Z_STRLEN_PP(element) == 0 || (Z_STRLEN_PP(element) == 1 && Z_STRVAL_PP(element)[0] == '0')) {
				return -1;
			}

			ser->serialize_bytes(Z_STRVAL_PP(element), Z_STRLEN_PP(element), scheme, container, outer TSRMLS_CC);
		case TYPE_UINT32:
		{
			uint32_t v;

			zval_copy_ctor(&value_copy);
			convert_to_uint32(&value_copy, &v);
			ser->serialize_uint32(v, scheme, container, outer TSRMLS_CC);
			zval_dtor(&value_copy);
			break;
		}
		case TYPE_ENUM:
			zval_copy_ctor(&value_copy);
			convert_to_long(&value_copy);

			ser->serialize_enum((int32_t)Z_LVAL_P(&value_copy), scheme, container, outer TSRMLS_CC);
			zval_dtor(&value_copy);
			break;
		break;
		case TYPE_SFIXED32:
		{
			int32_t v;

			zval_copy_ctor(&value_copy);
			convert_to_int32(&value_copy, &v);
			ser->serialize_sfixed32(v, scheme, container, outer TSRMLS_CC);
			zval_dtor(&value_copy);
			break;
		}
		case TYPE_SFIXED64:
		{
			int64_t v;

			zval_copy_ctor(&value_copy);
			convert_to_int64(&value_copy, &v);
			ser->serialize_sfixed64(v, scheme, container, outer TSRMLS_CC);
			zval_dtor(&value_copy);
			break;
		}
		case TYPE_SINT32:
		{
			int32_t v;

			zval_copy_ctor(&value_copy);
			convert_to_int32(&value_copy, &v);
			ser->serialize_sint32(v, scheme, container, outer TSRMLS_CC);
			zval_dtor(&value_copy);
			break;
		}
		case TYPE_SINT64:
		{
			int64_t v;

			zval_copy_ctor(&value_copy);
			convert_to_int64(&value_copy, &v);
			ser->serialize_sint64(v, scheme, container, outer TSRMLS_CC);
			zval_dtor(&value_copy);
			break;
		}
		break;
		default:
		break;
	}

	return 0;
}

static void php_protocolbuffers_json_encode_element(php_protocolbuffers_scheme_container *container, HashTable *hash, php_protocolbuffers_scheme *scheme, int throws_exception, zval *result TSRMLS_DC)
{
	zval **tmp = NULL;
	const char *name = {0};
	int name_len = 0;
	php_protocolbuffers_serializer2 *ser = &json_serializer;

	name = php_protocolbuffers_get_property_name(container, scheme, &name_len);

	if (zend_hash_find(hash, name, name_len, (void **)&tmp) == SUCCESS) {
		HashPosition pos;
		zval **element;

		if (scheme->repeated) {
			zval *outer; // TODO(chobie): abstraction

			if (Z_TYPE_PP(tmp) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_PP(tmp)) > 0) {
				ser->serialize_repeated_begin(scheme, container, (void*)&outer TSRMLS_CC);

				for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(tmp), &pos);
								zend_hash_get_current_data_ex(Z_ARRVAL_PP(tmp), (void **)&element, &pos) == SUCCESS;
								zend_hash_move_forward_ex(Z_ARRVAL_PP(tmp), &pos)
				) {
					if (Z_TYPE_PP(element) == IS_NULL) {
						continue;
					}

					php_protocolbuffers_json_encode_value(element, container, scheme, ser, throws_exception, (void*)outer TSRMLS_CC);
				}

				ser->serialize_repeated_end(result, scheme, container, outer TSRMLS_CC);
			}
		} else {
			if (scheme->required > 0 && Z_TYPE_PP(tmp) == IS_NULL) {
				php_protocolbuffers_raise_error_or_exception(php_protocol_buffers_uninitialized_message_exception_class_entry, E_WARNING, throws_exception, "the class does have required property `%s`.", scheme->name);
				return;
			}
			if (scheme->required == 0 && Z_TYPE_PP(tmp) == IS_NULL) {
				return;
			}
			if (scheme->ce != NULL && Z_TYPE_PP(tmp) != IS_OBJECT) {
				return;
			}
			if (Z_TYPE_PP(tmp) == IS_ARRAY) {
				//php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s is not repeated field but array given", scheme->name);
				return;
			}

			php_protocolbuffers_json_encode_value(tmp, container, scheme, ser, throws_exception, (void*)result TSRMLS_CC);
		}
	} else {
		if (scheme->required > 0) {
			php_protocolbuffers_raise_error_or_exception(php_protocol_buffers_invalid_protocolbuffers_exception_class_entry, E_WARNING, throws_exception, "the class does not declared required property `%s`. probably you missed declaration", scheme->name);
			return;
		}
	}
}

int php_protocolbuffers_fetch_element2(php_protocolbuffers_scheme_container *container, HashTable *hash, php_protocolbuffers_scheme *scheme, int throws_exception, zval **output TSRMLS_DC)
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
		*output = *tmp;
	} else {
		if (scheme->required > 0) {
			php_protocolbuffers_raise_error_or_exception(php_protocol_buffers_invalid_protocolbuffers_exception_class_entry, E_WARNING, 0, "the class does not declared required property `%s`. probably you missed declaration", scheme->name);
			return 1;
		}
	}

	return 0;
}


int php_protocolbuffers_encode_jsonserialize(zval *klass, php_protocolbuffers_scheme_container *container, int throws_exception, zval **result TSRMLS_DC)
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
			php_protocolbuffers_raise_error_or_exception(spl_ce_InvalidArgumentException, E_WARNING, throws_exception, "the class does not have `_properties` protected property.");
			return -1;
		}
	}

	for (i = 0; i < container->size; i++) {
		zval *tmp;
		scheme = &(container->scheme[i]);

		if (php_protocolbuffers_fetch_element2(container, hash, scheme, throws_exception, &tmp TSRMLS_CC)) {
			return -1;
		}
		php_protocolbuffers_json_encode_element(container, hash, scheme, throws_exception, target TSRMLS_CC);

		if (EG(exception)) {
			return 1;
		}
	}

	return 0;
}
