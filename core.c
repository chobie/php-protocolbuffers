#include "php_protocolbuffers.h"

#include "core.h"
#include "serializer.h"
#include "unknown_field.h"
#include "unknown_field_set.h"


static char *single_property_name_default = "_properties";
static int single_property_name_default_len = sizeof("_properties");
static char *unknown_property_name_default = "_unknown";
static int unknown_property_name_default_len = sizeof("_unknown");

static void pb_convert_msg(HashTable *proto, const char *klass, int klass_len, pb_scheme **scheme, int *size TSRMLS_DC);

static const char* pb_decode_message(INTERNAL_FUNCTION_PARAMETERS, const char *data, const char *data_end, pb_scheme_container *container, zval **result);


int php_protocolbuffers_read_protected_property(zval *instance, char *name, size_t name_len, zval **result TSRMLS_DC)
{
	zval **tmp;
	char *prop_name;
	int prop_name_len = 0;
	int retval = 0;

	zend_mangle_property_name(&prop_name, &prop_name_len, (char*)"*", 1, name, name_len, 0);
	if (zend_hash_find(Z_OBJPROP_P(instance), prop_name, prop_name_len, (void **)&tmp) == SUCCESS) {
		*result = *tmp;
		retval = 1;
	}
	efree(prop_name);

	return retval;
}

char *pb_get_default_single_property_name()
{
	return single_property_name_default;
}

int pb_get_default_single_property_name_len()
{
	return single_property_name_default_len;
}

char *pb_get_default_unknown_property_name()
{
	return unknown_property_name_default;
}

int pb_get_default_unknown_property_name_len()
{
	return unknown_property_name_default_len;
}

void pb_scheme_container_init(pb_scheme_container *container)
{
	memset(container, '\0', sizeof(pb_scheme_container));

	container->size = 0;
	container->scheme = NULL;
	container->use_single_property = 0;
	container->process_unknown_fields = 0;
	container->use_wakeup_and_sleep = 0;
	container->extension_cnt = 0;
	container->extensions = NULL;
	container->use_wakeup_and_sleep = 0;
	container->single_property_h = 0;
	container->orig_single_property_name     = pb_get_default_single_property_name();
	container->orig_single_property_name_len = pb_get_default_single_property_name_len();

	zend_mangle_property_name(&container->single_property_name,
		&container->single_property_name_len,
		(char*)"*",
		1,
		(char*)container->orig_single_property_name,
		container->orig_single_property_name_len,
		0
	);
	container->single_property_h = zend_inline_hash_func(container->single_property_name, container->single_property_name_len);
}


static void pb_convert_msg(HashTable *proto, const char *klass, int klass_len, pb_scheme **scheme, int *size TSRMLS_DC)
{
	int n = 0, sz = 0;
	zval *element = NULL;
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
			zval *tmp = NULL;
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

			if (zend_call_method(NULL, *ce, NULL, ZEND_STRS("getdescriptor")-1, &ret, 0, NULL, NULL  TSRMLS_CC)) {
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
					PHP_PROTOCOLBUFFERS_EXCEPTION_ERROR(EG(exception));
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
		pb_scheme_container_init(container);
		container->scheme = ischeme;
		container->size = scheme_size;

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
	char *unknown_name = {0};
	int unknown_name_len = 0;
	zval **un = NULL;
	unknown_value *val;
	php_protocolbuffers_unknown_field *p = NULL;

	if (container->use_single_property > 0) {
		unknown_name = pb_get_default_unknown_property_name();
		unknown_name_len = pb_get_default_unknown_property_name_len();
	} else {
		zend_mangle_property_name(&unknown_name, &unknown_name_len, (char*)"*", 1, (char*)pb_get_default_unknown_property_name(), pb_get_default_unknown_property_name_len(), 0);
	}

	if (zend_hash_find(hresult, (char*)unknown_name, unknown_name_len, (void **)&un) == SUCCESS) {
		if (php_pb_unknown_field_get_field(INTERNAL_FUNCTION_PARAM_PASSTHRU, *un, tag, unknown_name, unknown_name_len, &p)) {
			val = (unknown_value*)emalloc(sizeof(val));
			val->varint = value;

			zend_hash_next_index_insert(p->ht, (void *)&val, sizeof(unknown_value), NULL);
		} else {
			object_init_ex(dz, protocol_buffers_unknown_field_class_entry);
			php_pb_unknown_field_properties_init(dz TSRMLS_CC);
			php_pb_unknown_field_set_number(dz, tag TSRMLS_CC);
			php_pb_unknown_field_set_type(dz, wiretype TSRMLS_CC);

			p = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, dz);

			val = (unknown_value*)emalloc(sizeof(unknown_value));
			val->varint = value;
			zend_hash_next_index_insert(p->ht, (void *)&val, sizeof(unknown_value), NULL);

			php_pb_unknown_field_set_add_field(INTERNAL_FUNCTION_PARAM_PASSTHRU, *un, tag, unknown_name, unknown_name_len, dz);
		}
	}

	if (container->use_single_property < 1) {
		efree(unknown_name);
	}
}

void process_unknown_field_bytes(INTERNAL_FUNCTION_PARAMETERS, pb_scheme_container *container, HashTable *hresult, int tag, int wiretype, uint8_t *bytes, int length)
{
	char *unknown_name = {0};
	int unknown_name_len = 0;
	zval *dz = NULL, **unknown_fieldset = NULL;
	unknown_value *val = NULL;
	php_protocolbuffers_unknown_field *p = NULL;
	uint8_t *buffer = NULL;

	if (container->use_single_property > 0) {
		unknown_name     = pb_get_default_unknown_property_name();
		unknown_name_len = pb_get_default_unknown_property_name_len();
	} else {
		zend_mangle_property_name(&unknown_name, &unknown_name_len, (char*)"*", 1, (char*)pb_get_default_unknown_property_name(), pb_get_default_unknown_property_name_len(), 0);
	}

	if (zend_hash_find(hresult, (char*)unknown_name, unknown_name_len, (void **)&unknown_fieldset) == SUCCESS) {
		if (Z_OBJCE_PP(unknown_fieldset) != protocol_buffers_unknown_field_set_class_entry) {
			return;
		}

		if (php_pb_unknown_field_get_field(INTERNAL_FUNCTION_PARAM_PASSTHRU, *unknown_fieldset, tag, unknown_name, unknown_name_len, &p)) {
			val = (unknown_value*)emalloc(sizeof(unknown_value));
			buffer = (uint8_t*)emalloc(length);
			memcpy(buffer, bytes, length);

			val->buffer.val = buffer;
			val->buffer.len = length;

			zend_hash_next_index_insert(p->ht, (void *)&val, sizeof(val), NULL);
		} else {
			MAKE_STD_ZVAL(dz);
			object_init_ex(dz, protocol_buffers_unknown_field_class_entry);
			php_pb_unknown_field_properties_init(dz TSRMLS_CC);
			php_pb_unknown_field_set_number(dz, tag TSRMLS_CC);
			php_pb_unknown_field_set_type(dz, wiretype TSRMLS_CC);

			p = PHP_PROTOCOLBUFFERS_GET_OBJECT(php_protocolbuffers_unknown_field, dz);

			val = (unknown_value*)emalloc(sizeof(unknown_value));
			buffer = (uint8_t*)emalloc(length);
			memcpy(buffer, bytes, length);
			val->buffer.val = buffer;
			val->buffer.len = length;
			zend_hash_next_index_insert(p->ht, (void *)&val, sizeof(val), NULL);

			php_pb_unknown_field_set_add_field(INTERNAL_FUNCTION_PARAM_PASSTHRU, *unknown_fieldset, tag, unknown_name, unknown_name_len, dz);
		}
	}


	if (container->use_single_property < 1) {
		efree(unknown_name);
	}
}

void pb_format_string(zval *result, pbf *payload TSRMLS_DC)
{
	char __buffer[64] = {0};
	char *buffer = __buffer;
	int free = 0;
	size_t size = 0;

	switch (payload->type) {
		case TYPE_SINT32:
		case TYPE_INT32:
			size = snprintf(buffer, sizeof(__buffer), "%d", payload->value.int32);
		break;
		case TYPE_UINT32:
			size = snprintf(buffer, sizeof(__buffer), "%u", payload->value.uint32);
		break;
		case TYPE_UINT64:
			size = snprintf(buffer, sizeof(__buffer), "%llu" , payload->value.uint64);
		break;
		case TYPE_SINT64:
		case TYPE_INT64:
			size = snprintf(buffer, sizeof(__buffer), "%lld" , payload->value.int64);
		break;
		case TYPE_FLOAT:{
			char *p = 0;
			free = 1;

 			/* Note: this is safe */
			buffer = emalloc(MAX_LENGTH_OF_DOUBLE + EG(precision) + 1);
			size = zend_sprintf(buffer, "%f", payload->value.f);

			// remove trailing zeros
			p = strchr(buffer,'.');
			if(p != NULL) {
				while(*++p);

				while('0' == *--p){
					*p = '\0';
					size--;
				}

				if(*p == '.'){
					*p = '\0';
					size--;
				}
				break;
			}
  		}
		break;
		case TYPE_DOUBLE:{
			free = 1;
 			/* Note: this is safe */
			buffer = emalloc(MAX_LENGTH_OF_DOUBLE + EG(precision) + 1);
			size = zend_sprintf(buffer, "%.*G", (int)EG(precision), payload->value.d);
		}
		break;
		default:
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "the type %d does not support. maybe this is bug", payload->type);
			return;
		break;
	}

	if (buffer != NULL) {
		ZVAL_STRINGL(result, buffer, size, 1);
	}
	if (free) {
		efree(buffer);
	}
}

const char* pb_decode_message(INTERNAL_FUNCTION_PARAMETERS, const char *data, const char *data_end, pb_scheme_container *container, zval **result)
{
	uint32_t payload = 0, tag = 0, wiretype = 0;
	uint64_t value = 0;
	zval *dz = NULL;
	HashTable *hresult;
	pbf __payload = {0};

	if (container->use_single_property > 0) {
		zval **tmp = NULL;

		if (zend_hash_quick_find(Z_OBJPROP_PP(result), container->single_property_name, container->single_property_name_len+1, container->single_property_h, (void **)&tmp) == SUCCESS) {
			if (Z_TYPE_PP(tmp) != IS_ARRAY) {
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

		data = ReadVarint32FromArray(data, &payload, data_end);

		if (data == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_decode_message failed. ReadVarint32FromArray returns NULL.");
			return NULL;
		}

		tag      = (payload >> 0x03);
		wiretype = (payload & 0x07);

		if (tag < 1 || tag > ktagmax) {
			return NULL;
		}

		if (wiretype > WIRETYPE_FIXED32) {
			return NULL;
		}

		s = pb_search_scheme_by_tag(container->scheme, container->size, tag);

		switch (wiretype) {
		case WIRETYPE_VARINT:
		{
			data = ReadVarint64FromArray(data, &value, data_end);
			if (!pb_process_varint(INTERNAL_FUNCTION_PARAM_PASSTHRU, wiretype, tag, container, s, value, hresult)) {
				return NULL;
			}
		}
		break;
		case WIRETYPE_FIXED64:
			if (!pb_process_fixed64(INTERNAL_FUNCTION_PARAM_PASSTHRU, wiretype, tag, container, s, data, hresult)) {
            	return NULL;
			}
			data += 8;
		break;
		case WIRETYPE_LENGTH_DELIMITED:
			data = ReadVarint32FromArray(data, &payload, data_end);

			if ((data+payload) > data_end) {
				return NULL;
			}

			if (s == NULL) {
				if (container->process_unknown_fields > 0) {
					process_unknown_field_bytes(INTERNAL_FUNCTION_PARAM_PASSTHRU, container, hresult, tag, wiretype, data, payload);
				} else {
					/* skip unknown field */
				}
			} else if (s->type == TYPE_STRING) {
				MAKE_STD_ZVAL(dz);
				ZVAL_STRINGL(dz, (char*)data, payload, 1);

				php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);

			} else if (s->type == TYPE_BYTES) {
				MAKE_STD_ZVAL(dz);
				ZVAL_STRINGL(dz, (char*)data, payload, 1);

				php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);
			} else if (s->type == TYPE_MESSAGE) {
				const char *n_buffer_end = data + payload;
				zval *z_obj = NULL;
				pb_scheme_container *c_container = NULL;
				char *name = {0};
				int name_length = 0;
				ulong name_hash = 0;

				pb_get_scheme_container(s->ce->name, s->ce->name_length, &c_container, NULL TSRMLS_CC);

				MAKE_STD_ZVAL(z_obj);
				object_init_ex(z_obj, s->ce);
				php_pb_properties_init(z_obj, s->ce TSRMLS_CC);

				pb_decode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, data, n_buffer_end, c_container, &z_obj);

				if (c_container->use_wakeup_and_sleep > 0) {
					pb_execute_wakeup(z_obj, c_container TSRMLS_CC);
				}

				if (container->use_single_property < 1) {
					name        = s->mangled_name;
					name_length = s->mangled_name_len;
					name_hash   = s->mangled_name_h;
				} else {
					name        = s->name;
					name_length = s->name_len;
					name_hash   = s->name_h;
				}

				if (s->repeated) {
					if (!zend_hash_quick_exists(hresult, name, name_length, name_hash)) {
						zval *arr = NULL;

						MAKE_STD_ZVAL(arr);
						array_init(arr);

						Z_ADDREF_P(z_obj);
						zend_hash_next_index_insert(Z_ARRVAL_P(arr), (void *)&z_obj, sizeof(z_obj), NULL);
						Z_ADDREF_P(arr);
						zend_hash_update(hresult, name, name_length, (void **)&arr, sizeof(arr), NULL);
						zval_ptr_dtor(&arr);
					} else {
						zval **arr2 = NULL;

						if (zend_hash_quick_find(hresult, name, name_length, name_hash, (void **)&arr2) == SUCCESS) {
							Z_ADDREF_P(z_obj);
							zend_hash_next_index_insert(Z_ARRVAL_PP(arr2), (void *)&z_obj, sizeof(zval *), NULL);
						}
					}
				} else {
					zend_hash_quick_update(hresult, name, name_length, name_hash, (void **)&z_obj, sizeof(zval *), NULL);
					Z_ADDREF_P(z_obj);
				}

				zval_ptr_dtor(&z_obj);
			} else if (s->packed) {
				const char *packed_data_end, *last_data_offset;

				last_data_offset = data;
				packed_data_end = data + payload;
				do {
					switch (s->type) {
						case TYPE_DOUBLE:
						{
							uint64_t _v;
							double d;
							memcpy(&_v, data, 8);

							d = decode_double(_v);
							MAKE_STD_ZVAL(dz);

							__payload.type = TYPE_DOUBLE;__payload.value.d = d;
							pb_format_string(dz, &__payload TSRMLS_CC);
							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);

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

							__payload.type = TYPE_DOUBLE;__payload.value.d = a;
							pb_format_string(dz, &__payload TSRMLS_CC);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);

							data += 4;
						}
						break;
						case TYPE_INT64:
						{
							uint64_t v2;
							data = ReadVarint64FromArray(data, &v2, data_end);

							MAKE_STD_ZVAL(dz);

							__payload.type = TYPE_INT64;__payload.value.int64 = v2;
							pb_format_string(dz, &__payload TSRMLS_CC);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);
						}
						break;
						case TYPE_UINT64:
						{
							uint64_t v2;
							data = ReadVarint64FromArray(data, &v2, data_end);

							MAKE_STD_ZVAL(dz);

							__payload.type = TYPE_UINT64;__payload.value.uint64 = v2;
							pb_format_string(dz, &__payload TSRMLS_CC);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);
						}
						break;
						break;
						case TYPE_INT32:
							data = ReadVarint32FromArray(data, &payload, data_end);

							MAKE_STD_ZVAL(dz);

							__payload.type = TYPE_INT32;__payload.value.int32 = (int32_t)payload;
							pb_format_string(dz, &__payload TSRMLS_CC);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);
						break;
						case TYPE_FIXED64:
						{
							uint64_t l;
							memcpy(&l, data, 8);

							MAKE_STD_ZVAL(dz);
							__payload.type = TYPE_UINT64;__payload.value.uint64 = l;
							pb_format_string(dz, &__payload TSRMLS_CC);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);
							data += 8;
						}
						break;
						case TYPE_FIXED32:
						{
							uint32_t l = 0;
							memcpy(&l, data, 4);

							MAKE_STD_ZVAL(dz);
							__payload.type = TYPE_UINT32;__payload.value.uint32 = l;
							pb_format_string(dz, &__payload TSRMLS_CC);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);

							data += 4;
						}
						break;
						case TYPE_BOOL:
							data = ReadVarint32FromArray(data, &payload, data_end);

							MAKE_STD_ZVAL(dz);
							ZVAL_BOOL(dz, payload);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);
						break;
						case TYPE_UINT32:
							data = ReadVarint32FromArray(data, &payload, data_end);

							MAKE_STD_ZVAL(dz);
							__payload.type = TYPE_UINT32;__payload.value.uint32 = (uint32_t)payload;
							pb_format_string(dz, &__payload TSRMLS_CC);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);
						break;
						case TYPE_ENUM:
							data = ReadVarint32FromArray(data, &payload, data_end);

							MAKE_STD_ZVAL(dz);
							ZVAL_LONG(dz, (int32_t)payload);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);
						break;
						case TYPE_SFIXED32:
						{
							int32_t l = 0;

							memcpy(&l, data, 4);
							MAKE_STD_ZVAL(dz);

							__payload.type = TYPE_INT32;__payload.value.int32 = l;
							pb_format_string(dz, &__payload TSRMLS_CC);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);
							data += 4;
						}
						break;
						case TYPE_SFIXED64:
						{
							int64_t l;
							memcpy(&l, data, 8);

							MAKE_STD_ZVAL(dz);
							__payload.type = TYPE_INT64;__payload.value.int64 = l;
							pb_format_string(dz, &__payload TSRMLS_CC);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);

							data += 8;
						}
						break;
						case TYPE_SINT32:
							data = ReadVarint32FromArray(data, &payload, data_end);

							MAKE_STD_ZVAL(dz);
							__payload.type = TYPE_INT32;__payload.value.int32 = (int32_t)zigzag_decode32(payload);
							pb_format_string(dz, &__payload TSRMLS_CC);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);
						break;
						case TYPE_SINT64:
						{
							uint64_t v2;
							data = ReadVarint64FromArray(data, &v2, data_end);

							MAKE_STD_ZVAL(dz);

							__payload.type = TYPE_SINT64;__payload.value.int64 = (int64_t)zigzag_decode64(v2);
							pb_format_string(dz, &__payload TSRMLS_CC);

							php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz TSRMLS_CC);
						}
						break;
					}

					if (last_data_offset == data) {
						php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_decode_message: detect infinite loop when processing repeated packed field.");
						break;
					}
					last_data_offset = data;
				} while(data < packed_data_end);
				break;
			}

			data += payload;
		break;
		case WIRETYPE_FIXED32: {
			if (!pb_process_fixed32(INTERNAL_FUNCTION_PARAM_PASSTHRU, wiretype, tag, container, s, data, hresult)) {
				return NULL;
			}
			data += 4;
		}
		break;
		}
	}

	return data;
}



int php_protocolbuffers_encode(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *ce, zval *klass)
{
	int err = 0;
	pb_serializer *ser = NULL;
	pb_scheme_container *container;

	err = pb_get_scheme_container(ce->name, ce->name_length, &container, NULL TSRMLS_CC);
	if (err) {
		if (EG(exception)) {
			return 1;
		} else {
			/* TODO: improve displaying error message */
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_get_scheme_container failed. %s does not have getDescriptor method", ce->name);
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
	zval *obj = NULL;
	pb_scheme_container *container;
	const char *data_end, *res;
	int err = 0;

	if (data_len < 1) {
		zend_throw_exception_ex(protocol_buffers_invalid_protocolbuffers_exception_class_entry, 0 TSRMLS_CC, "passed variable seems null");
		return 1;
	}

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
			if (zend_hash_find(PBG(classes), (char*)klass, klass_len, (void **)&ce) == FAILURE) {
				zend_lookup_class((char*)klass, klass_len, &ce TSRMLS_CC);
				if (ce != NULL) {
					zend_hash_update(PBG(classes), (char*)klass, klass_len, (void **)ce, sizeof(ce), NULL);
				} else {
					php_error_docref(NULL TSRMLS_CC, E_ERROR, "class lookup failed. %s does exist", klass);
					return 1;
				}
			}
		}

		MAKE_STD_ZVAL(obj);
		object_init_ex(obj, *ce);
		php_pb_properties_init(obj, *ce TSRMLS_CC);

		/* add unknown fields */
		if (container->process_unknown_fields > 0) {
			zval **un = NULL, *unknown = NULL;
			char *unknown_name = {0};
			int unknown_name_len = 0;

			MAKE_STD_ZVAL(unknown);

			object_init_ex(unknown, protocol_buffers_unknown_field_set_class_entry);
			php_pb_unknown_field_set_properties_init(unknown TSRMLS_CC);

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

		if (container->use_wakeup_and_sleep > 0) {
			pb_execute_wakeup(obj, container TSRMLS_CC);
		}
	}

	RETVAL_ZVAL(obj, 0, 1);
	return 0;
}

void pb_execute_wakeup(zval *obj, pb_scheme_container *container TSRMLS_DC)
{
	zval fname, *retval_ptr = NULL;

	if (Z_OBJCE_P(obj) != PHP_IC_ENTRY &&
		zend_hash_exists(&Z_OBJCE_P(obj)->function_table, "__wakeup", sizeof("__wakeup"))) {

			INIT_PZVAL(&fname);
			ZVAL_STRINGL(&fname, "__wakeup", sizeof("__wakeup") -1, 0);

			call_user_function_ex(CG(function_table), &obj, &fname, &retval_ptr, 0, 0, 1, NULL TSRMLS_CC);
	}

	if (retval_ptr) {
		zval_ptr_dtor(&retval_ptr);
	}
}

void pb_execute_sleep(zval *obj, pb_scheme_container *container TSRMLS_DC)
{
	zval fname, *retval_ptr = NULL;

	if (Z_OBJCE_P(obj) != PHP_IC_ENTRY &&
		zend_hash_exists(&Z_OBJCE_P(obj)->function_table, "__sleep", sizeof("__sleep"))) {

		INIT_PZVAL(&fname);
		ZVAL_STRINGL(&fname, "__sleep", sizeof("__sleep") -1, 0);

		call_user_function_ex(CG(function_table), &obj, &fname, &retval_ptr, 0, 0, 1, NULL TSRMLS_CC);

		if (retval_ptr) {
			if (Z_TYPE_P(retval_ptr) != IS_ARRAY) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_execute_sleep failed. __sleep method have to return an array");
				zval_ptr_dtor(&retval_ptr);
				retval_ptr = NULL;
			}
		} else {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_execute_sleep failed. __sleep method have to return an array");
		}
	}

	if (retval_ptr) {
		zval **entry = NULL;
		HashPosition pos;
		pb_scheme *scheme;
		int i;

		for (i = 0; i < container->size; i++) {
			scheme = &(container->scheme[i]);
			scheme->skip = 1;
		}

		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(retval_ptr), &pos);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(retval_ptr), (void **)&entry, &pos) == SUCCESS) {
			for (i = 0; i < container->size; i++) {
				if (Z_TYPE_PP(entry) != IS_STRING) {
					convert_to_string(*entry);
				}

				scheme = &(container->scheme[i]);
				if (strcmp(scheme->name, Z_STRVAL_PP(entry)) == 0) {
					scheme->skip = 0;
				}
			}

			zend_hash_move_forward_ex(Z_ARRVAL_P(retval_ptr), &pos);
		}

		zval_ptr_dtor(&retval_ptr);
	}
}

int php_pb_properties_init(zval *object, zend_class_entry *ce TSRMLS_DC)
{
	zval *pp = NULL;
	int j = 0;
	pb_scheme_container *container = NULL;
	pb_scheme *scheme = NULL;
	HashTable *properties = NULL, *proto = NULL;

	pb_get_scheme_container(ce->name, ce->name_length, &container, proto TSRMLS_CC);
	ALLOC_HASHTABLE(properties);
	zend_hash_init(properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	if (container->use_single_property > 0) {
		MAKE_STD_ZVAL(pp);
		array_init(pp);
		zend_hash_update(properties, container->orig_single_property_name, container->orig_single_property_name_len, (void **)&pp, sizeof(zval), NULL);
	} else {
		for (j = 0; j < container->size; j++) {
			scheme= &container->scheme[j];
			MAKE_STD_ZVAL(pp);

			if (scheme->repeated > 0) {
				array_init(pp);
			} else {
				ZVAL_NULL(pp);
			}

			zend_hash_update(properties, scheme->name, scheme->name_len, (void **)&pp, sizeof(zval), NULL);
		}
	}

	zend_merge_properties(object, properties, 1 TSRMLS_CC);
	return 0;
}
