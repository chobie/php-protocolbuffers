/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Shuhei Tanuma <chobieee@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "php_protocol_buffers.h"
#include "helper.h"
#include "protocol_buffers.h"
extern "C" {
#include "ext/standard/php_var.h"
}

#ifdef ZTS
PHPAPI int pb_globals_id;
#else
PHPAPI pb_globals pb_globals;
#endif

static const int kMaxVarintBytes = 10;
static const int kMaxVarint32Bytes = 5;

zend_class_entry *protocol_buffers_class_entry;


void messages_dtor(pb_scheme *entry)
{
}

static void pb_globals_ctor(pb_globals *pb_globals_p TSRMLS_DC)
{
}

static void pb_globals_dtor(pb_globals *pb_globals_p TSRMLS_DC) /* {{{ */
{
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, class_name)
	ZEND_ARG_INFO(0, bytes)
	ZEND_ARG_INFO(0, descriptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_encode, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, descriptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_read_varint32, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

PHP_MINFO_FUNCTION(protocolbuffers)
{
}

PHP_MINIT_FUNCTION(protocolbuffers)
{

#ifdef ZTS
    ts_allocate_id(&pb_globals_id, sizeof(pb_globals), (ts_allocate_ctor) pb_globals_ctor, (ts_allocate_dtor) pb_globals_dtor);
#ifdef PHP_WIN32
    ts_allocate_id(&php_win32_core_globals_id, sizeof(php_win32_core_globals), (ts_allocate_ctor)php_win32_core_globals_ctor, (ts_allocate_dtor)php_win32_core_globals_dtor);
#endif
#else
    pb_globals_ctor(&pb_globals TSRMLS_CC);
#ifdef PHP_WIN32
    php_win32_core_globals_ctor(&the_php_win32_core_globals TSRMLS_CC);
#endif
#endif

	php_protocolbuffers_init(TSRMLS_C);
	return SUCCESS;
}

PHP_RINIT_FUNCTION(protocolbuffers)
{
    PBG(messages) = NULL;
    if (!PBG(messages)) {
            ALLOC_HASHTABLE(PBG(messages));
            zend_hash_init(PBG(messages), 0, NULL, (void (*)(void *)) messages_dtor, 0);
    }

    return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(protocolbuffers)
{
#ifdef ZTS
        ts_free_id(pb_globals_id);
#ifdef PHP_WIN32
        ts_free_id(php_win32_core_globals_id);
#endif
#else
        basic_globals_dtor(&pb_globals TSRMLS_CC);
#ifdef PHP_WIN32
        php_win32_core_globals_dtor(&the_php_win32_core_globals TSRMLS_CC);
#endif
#endif

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(protocolbuffers)
{
    if (PBG(messages)) {
        zend_try {
            zend_hash_destroy(PBG(messages));
            FREE_HASHTABLE(PBG(messages));
            PBG(messages) = NULL;
        }
        zend_end_try();
    }

	return SUCCESS;
}


static inline const char* ReadVarint32FromArray(const char* buffer, uint* value, const char* buffer_end) {
  // Fast path:  We have enough bytes left in the buffer to guarantee that
  // this read won't cross the end, so we can skip the checks.
  const char* ptr = buffer;
  int b;
  int result;

  if (GOOGLE_PREDICT_TRUE(buffer < buffer_end) && *buffer < 0x80) {
    *value = *buffer;
    ptr++;
    return ptr;
  }

  b = *(ptr++); result  = (b & 0x7F)      ; if (!(b & 0x80)) goto done;
  b = *(ptr++); result |= (b & 0x7F) <<  7; if (!(b & 0x80)) goto done;
  b = *(ptr++); result |= (b & 0x7F) << 14; if (!(b & 0x80)) goto done;
  b = *(ptr++); result |= (b & 0x7F) << 21; if (!(b & 0x80)) goto done;
  b = *(ptr++); result |=  b         << 28; if (!(b & 0x80)) goto done;

  // If the input is larger than 32 bits, we still need to read it all
  // and discard the high-order bits.
  for (int i = 0; i < kMaxVarintBytes - kMaxVarint32Bytes; i++) {
    b = *(ptr++); if (!(b & 0x80)) goto done;
  }

  // We have overrun the maximum size of a varint (10 bytes).  Assume
  // the data is corrupt.
  return NULL;

 done:
  *value = result;
  return ptr;
}

static int pb_get_tag_name(HashTable *proto, ulong tag, zval **result TSRMLS_DC)
{
    zval **d, **dd;

    if (zend_hash_index_find(proto, tag, (void **)&d) != SUCCESS) {
        return 0;
    }
    if (Z_TYPE_PP(d) != IS_ARRAY) {
        return 0;
    }

    if (zend_hash_find(Z_ARRVAL_PP(d), "name", sizeof("name"), (void **)&dd) == SUCCESS) {
        *result = *dd;
        return 1;
    }
    return 0;
}

static int pb_get_repeated(HashTable *proto, ulong tag TSRMLS_DC)
{
    zval **d, **dd;

    if (zend_hash_index_find(proto, tag, (void **)&d) != SUCCESS) {
        return 0;
    }
    if (Z_TYPE_PP(d) != IS_ARRAY) {
        return 0;
    }

    if (zend_hash_find(Z_ARRVAL_PP(d), "repeated", sizeof("repeated"), (void **)&dd) == SUCCESS) {
        return Z_LVAL_PP(dd);
    }
    return 0;
}


static int pb_get_msg_name(HashTable *proto, ulong tag, zval **result TSRMLS_DC)
{
    zval **d, **dd;

    if (zend_hash_index_find(proto, tag, (void **)&d) != SUCCESS) {
        return 0;
    }
    if (Z_TYPE_PP(d) != IS_ARRAY) {
        return 0;
    }

    if (zend_hash_find(Z_ARRVAL_PP(d), "message", sizeof("message"), (void **)&dd) == SUCCESS) {
        *result = *dd;
        return 1;
    }
    return 0;
}


static int pb_tag_is_string(HashTable *proto, ulong tag TSRMLS_DC)
{
    zval **d, **dd;

    if (zend_hash_index_find(proto, tag, (void **)&d) != SUCCESS) {
        return 0;
    }
    if (Z_TYPE_PP(d) != IS_ARRAY) {
        return 0;
    }
    if (zend_hash_find(Z_ARRVAL_PP(d), "type", sizeof("type"), (void **)&dd) == SUCCESS) {
        if (Z_LVAL_PP(dd) == TYPE_STRING) {
            return 1;
        }
    }

    return 0;
}

static int pb_tag_is_message(HashTable *proto, ulong tag TSRMLS_DC)
{
    zval **d, **dd;

    if (zend_hash_index_find(proto, tag, (void **)&d) != SUCCESS) {
        return 0;
    }
    if (Z_TYPE_PP(d) != IS_ARRAY) {
        return 0;
    }
    if (zend_hash_find(Z_ARRVAL_PP(d), "type", sizeof("type"), (void **)&dd) == SUCCESS) {
        if (Z_LVAL_PP(dd) == TYPE_MESSAGE) {
            return 1;
        }
    }

    return 0;
}

static int pb_tag_type(HashTable *proto, ulong tag TSRMLS_DC)
{
    zval **d, **dd;

    if (zend_hash_index_find(proto, tag, (void **)&d) != SUCCESS) {
        return -1;
    }
    if (Z_TYPE_PP(d) != IS_ARRAY) {
        return -1;
    }

    if (zend_hash_find(Z_ARRVAL_PP(d), "type", sizeof("type"), (void **)&dd) == SUCCESS) {
        return Z_LVAL_PP(dd);
    }

    return -1;
}

static int pb_tag_wiretype(HashTable *proto, ulong tag TSRMLS_DC)
{
    zval **d, **dd;

    if (zend_hash_index_find(proto, tag, (void **)&d) != SUCCESS) {
        return -1;
    }
    if (Z_TYPE_PP(d) != IS_ARRAY) {
        return -1;
    }

    if (zend_hash_find(Z_ARRVAL_PP(d), "wire_type", sizeof("wire_type"), (void **)&dd) == SUCCESS) {
        return Z_LVAL_PP(dd);
    }

    return -1;
}


static inline pb_scheme *pb_search_scheme_by_tag(pb_scheme* scheme, uint scheme_size, uint tag)
{
    int i = 0;

    for (i = 0; i < scheme_size; i++) {
        if (scheme[i].tag == tag) {
            return &scheme[i];
        }
    }

    fprintf(stderr, "TAG:%d NOTFOUND!", tag);
    return NULL;
}

static void pb_convert_msg(HashTable *proto, const char *klass, int klass_len, pb_scheme **scheme, int *size TSRMLS_DC)
{
    int n, sz;
    zval *element;
    HashPosition pos;
    pb_scheme *ischeme;

    sz = zend_hash_num_elements(proto);
    ischeme = (pb_scheme*)malloc(sizeof(pb_scheme) * sz);

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

            ischeme[n].type = pb_tag_type(proto, ttag TSRMLS_CC);

            pb_get_tag_name(proto, ttag, &tmp TSRMLS_CC);
            tsize = Z_STRLEN_P(tmp)+1;
            // TODO: fix memory leak
            ischeme[n].name = (char*)malloc(sizeof(char*) * tsize);
            ischeme[n].name_len = tsize;
            memcpy(ischeme[n].name, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
            ischeme[n].name[Z_STRLEN_P(tmp)+1] = '\0';
            ischeme[n].repeated = pb_get_repeated(proto, ttag TSRMLS_CC);

            if (ischeme[n].type == TYPE_MESSAGE) {
                pb_get_msg_name(proto, ttag, &tmp TSRMLS_CC);
                zend_lookup_class(Z_STRVAL_P(tmp), Z_STRLEN_P(tmp), &c TSRMLS_CC);
                ischeme[n].ce = *c;
            }
        }
    }

    *scheme = ischeme;
    *size = sz;
}

static int pb_get_scheme_container(const char *klass, size_t klass_len, pb_scheme_container **result, HashTable *descriptor TSRMLS_DC)
{
    pb_scheme_container *container, **cn;
    pb_scheme *ischeme;
    int scheme_size = 0;

    if (zend_hash_find(PBG(messages), klass, klass_len, (void **)&cn) != SUCCESS) {
        zval *ret = NULL;
        HashTable *proto = NULL;


        if (descriptor == NULL) {
            zend_class_entry **ce;

            zend_lookup_class(klass, klass_len, &ce TSRMLS_CC);
            if (zend_call_method(NULL, *ce, NULL, "getdescriptor", strlen("getdescriptor"), &ret, 0, NULL, NULL  TSRMLS_CC)) {
                proto = Z_ARRVAL_P(ret);
            } else {
                fprintf(stderr, "ERROR");
                return 1;
            }
        } else {
            proto = descriptor;
        }

        pb_convert_msg(proto, klass, klass_len, &ischeme, &scheme_size TSRMLS_CC);
        scheme_size = zend_hash_num_elements(proto);

        /* TODO: fix leak */
        container = (pb_scheme_container*)malloc(sizeof(pb_scheme_container));
        container->scheme = ischeme;
        container->size = scheme_size;

        zend_hash_add(PBG(messages), klass, klass_len, (void**)&container, sizeof(pb_scheme_container), NULL);

        if (ret != NULL) {
            zval_ptr_dtor(&ret);
        }
    } else {
        container = *cn;
    }

    *result = container;

    return 0;
}

static const char* pb_decode_message(INTERNAL_FUNCTION_PARAMETERS, const char *data, const char *data_end, pb_scheme_container *container, zval **result)
{
    uint value = 0, tag = 0, wiretype = 0;
    zval *tmp, *dz;
    HashTable *hresult;
    char buffer[512] = {0};

    MAKE_STD_ZVAL(tmp);
    array_init(tmp);
    hresult = Z_ARRVAL_P(tmp);

    while (data < data_end) {
        pb_scheme *s;

        data = ReadVarint32FromArray(data, &value, data_end);

        if (data == NULL) {
            fprintf(stderr, "ERROR!");
            return "";
        }

        tag      = (value >> 0x03);
        wiretype = (value & 0x07);

        s = pb_search_scheme_by_tag(container->scheme, container->size, tag);

        switch (wiretype) {
        case WIRETYPE_VARINT:
        {
            data = ReadVarint32FromArray(data, &value, data_end);
            MAKE_STD_ZVAL(dz);
            ZVAL_LONG(dz, value);

            zend_hash_add(hresult, s->name, s->name_len, (void **)&dz, sizeof(dz), NULL);
            Z_ADDREF_P(dz);
            zval_ptr_dtor(&dz);
        }
        break;
        case WIRETYPE_FIXED64:
            // TODO: implement this
            data += 8;
        break;
        case WIRETYPE_LENGTH_DELIMITED:
            data = ReadVarint32FromArray(data, &value, data_end);

            if (s->type == TYPE_STRING) {
                if (value < 512) {
                    memcpy(buffer, data, value);
                    buffer[value] = '\0';
                } else {
                    char *sub_buffer;

                    sub_buffer = (char *)emalloc(value+1);
                    memcpy(sub_buffer, data, value);
                    sub_buffer[value+1] = '\0';
                }

                MAKE_STD_ZVAL(dz);
                ZVAL_STRINGL(dz, data, value, 1);

                zend_hash_add(hresult, s->name, s->name_len, (void **)&dz, sizeof(dz), NULL);
                Z_ADDREF_P(dz);
                zval_ptr_dtor(&dz);
            } else if (s->type == TYPE_MESSAGE) {
                const char *n_buffer_end = data + value;
                zval *z_arr, *z_obj;
                pb_scheme_container *c_container;

                if (s->repeated) {
                    pb_get_scheme_container(s->ce->name, s->ce->name_length, &c_container, NULL TSRMLS_CC);
                    pb_decode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, data, n_buffer_end, c_container, &z_arr);
                    MAKE_STD_ZVAL(z_obj);

                    object_init_ex(z_obj, s->ce);
                    zend_hash_update(Z_OBJPROP_P(z_obj), "_properties", sizeof("_properties"), (void **)&z_arr, sizeof(zval*), NULL);

                    if (!zend_hash_exists(hresult, s->name, s->name_len)) {
                        zval *arr;

                        MAKE_STD_ZVAL(arr);
                        array_init(arr);

                        zend_hash_next_index_insert(Z_ARRVAL_P(arr), (void *)&z_obj, sizeof(z_obj), NULL);
                        Z_ADDREF_P(z_obj);

                        zend_hash_add(hresult, s->name, s->name_len, (void **)&arr, sizeof(arr), NULL);
                        Z_ADDREF_P(arr);
                        zval_ptr_dtor(&arr);

                    } else {
                        zval **arr2;

                        if (zend_hash_find(hresult, s->name, s->name_len, (void **)&arr2) == SUCCESS) {
                            zend_hash_next_index_insert(Z_ARRVAL_PP(arr2), (void *)&z_obj, sizeof(z_obj), NULL);
                            Z_ADDREF_P(z_obj);
                        }
                    }

                    zval_ptr_dtor(&z_obj);
                } else {
                    pb_get_scheme_container(s->ce->name, s->ce->name_length, &c_container, NULL TSRMLS_CC);
                    pb_decode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, data, n_buffer_end, c_container, &z_arr);
                    MAKE_STD_ZVAL(z_obj);

                    object_init_ex(z_obj, s->ce);
                    zend_hash_update(Z_OBJPROP_P(z_obj), "_properties", sizeof("_properties"), (void **)&z_arr, sizeof(zval*), NULL);

                    zend_hash_add(hresult, s->name, s->name_len, (void **)&z_obj, sizeof(z_obj), NULL);
                    Z_ADDREF_P(z_obj);
                    zval_ptr_dtor(&z_obj);
                }
            }

            data += value;
        break;
        case WIRETYPE_FIXED32: {
            if (s->type == TYPE_FLOAT) {
                float a;

                memcpy(&a, data, 4);

                MAKE_STD_ZVAL(dz);
                ZVAL_DOUBLE(dz, a);

                zend_hash_add(hresult, s->name, s->name_len, (void **)&dz, sizeof(dz), NULL);
                Z_ADDREF_P(dz);
                zval_ptr_dtor(&dz);
            }

            data += 4;
        }
        break;
        }
    }

    *result = tmp;

    return data;
}

static int pb_encode_message(INTERNAL_FUNCTION_PARAMETERS, zval *klass, pb_scheme_container *container, pb_serializer **serializer)
{
    int i = 0;
    pb_serializer *ser;
    zval **c;
    zval **tmp;
    HashTable *hash = NULL;

    pb_serializer_init(&ser);

    // TODO: mangle property name
    if (zend_hash_find(Z_OBJPROP_P(klass), "_properties", sizeof("_properties"), (void**)&c) == SUCCESS) {
        hash = Z_ARRVAL_PP(c);
    } else {
        fprintf(stderr, "_properties not found");
        return -1;
    }

    for (i = 0; i < container->size; i++) {
        pb_scheme *scheme;
        scheme = &(container->scheme[i]);

        switch (scheme->type) {
            case TYPE_DOUBLE:
            {
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    union {
                        double d;
                        uint64_t u;
                    } u;
                    u.d = Z_DVAL_PP(tmp);

                    pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED32);
                    pb_serializer_write64_le(ser, u.u);
                }
            }
            break;
            case TYPE_FLOAT:
            {
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    union {
                        float f;
                        uint32_t u;
                    } x;

                    x.f = (float)Z_DVAL_PP(tmp);

                    pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED32);
                    pb_serializer_write32_le(ser, x.u);
                }

            }
            break;
            case TYPE_INT64:
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
                    pb_serializer_write_varint64(ser, Z_LVAL_PP(tmp));
                }
            break;
            case TYPE_INT32:
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
                    pb_serializer_write_varint32(ser, Z_LVAL_PP(tmp));
                }
            break;
            case TYPE_FIXED64:
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED64);
                    pb_serializer_write64_le(ser, Z_LVAL_PP(tmp));
                }
            break;
            case TYPE_FIXED32:
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED32);
                    pb_serializer_write32_le(ser, Z_LVAL_PP(tmp));
                }
            break;
            case TYPE_BOOL:
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
                    pb_serializer_write_varint32(ser, Z_LVAL_PP(tmp));
                }
            break;
            case TYPE_STRING:
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    if (Z_STRLEN_PP(tmp) > 0) {
                        pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_LENGTH_DELIMITED);
                        pb_serializer_write_varint32(ser, Z_STRLEN_PP(tmp));
                        pb_serializer_write_chararray(ser, (unsigned char*)Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
                    }
                }
            break;
            case TYPE_GROUP:
            break;
            case TYPE_MESSAGE:
            {
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                        zend_class_entry *ce;
                        pb_scheme_container *n_container;
                        pb_serializer *n_ser = NULL;

                        if (scheme->repeated) {
                            HashPosition pos;
                            zval **element;

                            for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(tmp), &pos);
                                            zend_hash_get_current_data_ex(Z_ARRVAL_PP(tmp), (void **)&element, &pos) == SUCCESS;
                                            zend_hash_move_forward_ex(Z_ARRVAL_PP(tmp), &pos)
                            ) {
                                ce = Z_OBJCE_PP(element);

                                pb_get_scheme_container(ce->name, ce->name_length, &n_container, NULL TSRMLS_CC);
                                pb_encode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, *element, n_container, &n_ser);
                                pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_LENGTH_DELIMITED);
                                pb_serializer_write_varint32(ser, n_ser->buffer_size);
                                pb_serializer_write_chararray(ser, n_ser->buffer, n_ser->buffer_size);

                                efree(n_ser->buffer);
                                efree(n_ser);
                            }

                        } else {
                            ce = Z_OBJCE_PP(tmp);

                            pb_get_scheme_container(ce->name, ce->name_length, &n_container, NULL TSRMLS_CC);
                            pb_encode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, *tmp, n_container, &n_ser);
                            pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_LENGTH_DELIMITED);
                            pb_serializer_write_varint32(ser, n_ser->buffer_size);
                            pb_serializer_write_chararray(ser, n_ser->buffer, n_ser->buffer_size);

                            efree(n_ser->buffer);
                            efree(n_ser);
                        }

                }
            }
            break;
            case TYPE_BYTES:
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    if (Z_STRLEN_PP(tmp) > 0) {
                        pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_LENGTH_DELIMITED);
                        pb_serializer_write_varint32(ser, Z_STRLEN_PP(tmp));
                        pb_serializer_write_chararray(ser, (unsigned char*)Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
                    }
                }
            break;
            case TYPE_UINT32:
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
                    pb_serializer_write_varint32(ser, Z_LVAL_PP(tmp));
                }
            break;
            case TYPE_ENUM:
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
                    pb_serializer_write_varint32(ser, Z_LVAL_PP(tmp));
                }
            break;
            case TYPE_SFIXED32:
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED32);
                    pb_serializer_write32_le(ser, zigzag_encode32(Z_LVAL_PP(tmp)));
                }
            break;
            case TYPE_SFIXED64:
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED64);
                    pb_serializer_write64_le(ser, zigzag_encode32(Z_LVAL_PP(tmp)));
                }
            break;
            case TYPE_SINT32:
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED32);
                    pb_serializer_write_varint32(ser, zigzag_encode32(Z_LVAL_PP(tmp)));
                }
            break;
            case TYPE_SINT64:
                if (zend_hash_find(hash, scheme->name, scheme->name_len, (void **)&tmp) == SUCCESS) {
                    pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED64);
                    pb_serializer_write_varint64(ser, zigzag_encode64(Z_LVAL_PP(tmp)));
                }
            break;
            default:
            break;
        }
    }

    *serializer = ser;
    return 0;
}

PHP_METHOD(protocolbuffers, decode)
{
    HashTable *proto = NULL;
    char *klass;
    const char *data, *data_end;
    long klass_len = 0, data_len = 0;
    long buffer_size = 0;
    zval *z_result, *z_proto = NULL;
    zval *obj;
    pb_scheme_container *container;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"ss|a", &klass, &klass_len, &data, &data_len, &z_proto) == FAILURE) {
		return;
	}

    if (z_proto) {
        proto       = Z_ARRVAL_P(z_proto);
    }

    buffer_size = (long)data + sizeof(data);
    pb_get_scheme_container(klass, klass_len, &container, proto TSRMLS_CC);
    data_end = data + data_len;
    pb_decode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, data, data_end, container, &z_result);

    {
        zval func;

        zval **pp[1];
        zend_class_entry **ce;

        MAKE_STD_ZVAL(obj);
        zend_lookup_class(klass, klass_len, &ce TSRMLS_CC);
        pp[0] = &z_result;

        ZVAL_STRINGL(&func, "__construct", sizeof("__construct") - 1, 0);

        object_init_ex(obj, *ce);

        //call_user_function_ex(NULL, &obj, &func, &ret, 1, pp, 0, NULL  TSRMLS_CC);
        //Z_OBJPROP_P(obj)
        //zend_mangle_property_name(char **dest, int *dest_length, const char *src1, int src1_length, const char *src2, int src2_length, int internal)
        //zend_mangle_property_name(&kkey, &klen, "*", 1, "_properties", sizeof("_properties")+1, 0);
        //zend_hash_add(Z_OBJPROP_P(obj), kkey, klen, (void **)&z_result, sizeof(zval*), NULL);

        //zend_hash_find(Z_OBJPROP_P(obj), "_properties", sizeof("_properties")+1, (void **)&h);
        zend_hash_update(Z_OBJPROP_P(obj), "_properties", sizeof("_properties"), (void **)&z_result, sizeof(zval*), NULL);

        //zend_hash_update(Z_OBJPROP_P(obj), kkey, klen, (void **)&z_result, sizeof(zval*), NULL);
        Z_ADDREF_P(z_result);
        zval_ptr_dtor(&z_result);

    }

    RETURN_ZVAL(obj, 0, 1);
}

static void pb_serializer_init(pb_serializer **serializer)
{
    pb_serializer *ser;
    ser = (pb_serializer*)emalloc(sizeof(pb_serializer));

    ser->buffer_size = 0;
    ser->buffer_capacity = 32;
    ser->buffer_offset = 0;
    ser->buffer = (uint8_t*)emalloc(sizeof(uint8_t) * ser->buffer_capacity);
    memset(ser->buffer, '\0', ser->buffer_capacity);

    *serializer = ser;
}

static int pb_serializer_resize(pb_serializer *serializer, size_t size)
{
	if (serializer->buffer_size + size < serializer->buffer_capacity) {
		return 0;
	}

	while (serializer->buffer_size + size >= serializer->buffer_capacity) {
		serializer->buffer_capacity *= 2;
	}

	serializer->buffer = (uint8_t*)erealloc(serializer->buffer, serializer->buffer_capacity);
	if (serializer->buffer == NULL)
		return 1;

	return 0;
}

static int pb_serializer_write8(pb_serializer *serializer, unsigned int value)
{
    if (pb_serializer_resize(serializer, 1)) {
        return 1;
    }

    serializer->buffer[serializer->buffer_size++] = value;
    return 0;
}

static int pb_serializer_write16(pb_serializer *serializer, unsigned int value)
{
    if (pb_serializer_resize(serializer, 1)) {
        return 1;
    }

	serializer->buffer[serializer->buffer_size++] = (unsigned char) (value >> 8 & 0xff);
	serializer->buffer[serializer->buffer_size++] = (unsigned char) (value & 0xff);

    return 0;
}

static int pb_serializer_write32(pb_serializer *serializer, unsigned int value)
{
    if (pb_serializer_resize(serializer, 1)) {
        return 1;
    }

	serializer->buffer[serializer->buffer_size++] = (uint8_t) (value >> 24 & 0xff);
	serializer->buffer[serializer->buffer_size++] = (uint8_t) (value >> 16 & 0xff);
	serializer->buffer[serializer->buffer_size++] = (uint8_t) (value >> 8 & 0xff);
	serializer->buffer[serializer->buffer_size++] = (uint8_t) (value & 0xff);

    return 0;
}

static int pb_serializer_write32_le(pb_serializer *serializer, unsigned int value)
{
    unsigned int target[4];

    if (pb_serializer_resize(serializer, 1)) {
        return 1;
    }

#ifdef PROTOBUF_LITTLE_ENDIAN
    memcpy(target, value, 4);
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
    unsigned int target[8];

    if (pb_serializer_resize(serializer, 1)) {
        return 1;
    }

#ifdef PROTOBUF_LITTLE_ENDIAN
    memcpy(target, value, 8);
#else
    target[0] = (value);
    target[1] = (value >>  8);
    target[2] = (value >> 16);
    target[3] = (value >> 24);
    target[4] = (value >> 32);
    target[5] = (value >> 40);
    target[6] = (value >> 48);
    target[7] = (value >> 52);
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


static int pb_serializer_write64(pb_serializer *serializer, uint64_t value)
{
    if (pb_serializer_resize(serializer, 1)) {
        return 1;
    }

	serializer->buffer[serializer->buffer_size++] = (uint8_t) (value >> 56 & 0xff);
	serializer->buffer[serializer->buffer_size++] = (uint8_t) (value >> 48 & 0xff);
	serializer->buffer[serializer->buffer_size++] = (uint8_t) (value >> 40 & 0xff);
	serializer->buffer[serializer->buffer_size++] = (uint8_t) (value >> 32 & 0xff);
	serializer->buffer[serializer->buffer_size++] = (uint8_t) (value >> 24 & 0xff);
	serializer->buffer[serializer->buffer_size++] = (uint8_t) (value >> 16 & 0xff);
	serializer->buffer[serializer->buffer_size++] = (uint8_t) (value >> 8 & 0xff);
	serializer->buffer[serializer->buffer_size++] = (uint8_t) (value & 0xff);

    return 0;
}

static int pb_serializer_write_varint32(pb_serializer *serializer, uint8_t value)
{
    uint8_t bytes[kMaxVarint32Bytes];
    int size = 0, i;

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


static int pb_serializer_write_chararray(pb_serializer *serializer, unsigned char *string, size_t len)
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

PHP_METHOD(protocolbuffers, encode)
{
    zval *klass, *z_descriptor = NULL;
    zend_class_entry *ce;
    pb_scheme_container *container;
    HashTable *proto = NULL;
    pb_serializer *ser;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"o|a", &klass, &z_descriptor) == FAILURE) {
		return;
	}

	ce = Z_OBJCE_P(klass);
    if (z_descriptor) {
        proto       = Z_ARRVAL_P(z_descriptor);
    }

    pb_get_scheme_container(ce->name, ce->name_length, &container, proto TSRMLS_CC);
    pb_encode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, klass, container, &ser);

    if (ser->buffer_capacity > 0) {
        RETVAL_STRINGL((const char*)ser->buffer, ser->buffer_size, 1);

        efree(ser->buffer);
        efree(ser);
    } else {
        fprintf(stderr, "NOTHING\n");
        RETURN_EMPTY_STRING();
    }

}

static zend_function_entry php_protocolbuffers_methods[] = {
    PHP_ME(protocolbuffers, decode, arginfo_pb_decode, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers, encode, NULL, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};


void php_protocolbuffers_init(TSRMLS_D)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "ProtocolBuffers", php_protocolbuffers_methods);
    protocol_buffers_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_class_constant_long(protocol_buffers_class_entry, "WIRETYPE_VARINT", sizeof("WIRETYPE_VARINT")-1, 0 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "WIRETYPE_FIXED64", sizeof("WIRETYPE_FIXED64")-1, 1 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "WIRETYPE_LENGTH_DELIMITED", sizeof("WIRETYPE_LENGTH_DELIMITED")-1, 2 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "WIRETYPE_START_GROUP", sizeof("WIRETYPE_START_GROUP")-1, 3 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "WIRETYPE_END_GROUP", sizeof("WIRETYPE_END_GROUP")-1, 4 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "WIRETYPE_FIXED32", sizeof("WIRETYPE_FIXED32")-1, 5 TSRMLS_CC);

    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_DOUBLE", sizeof("TYPE_DOUBLE")-1, 1 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_FLOAT", sizeof("TYPE_FLOAT")-1, 2 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_INT64", sizeof("TYPE_INT64")-1, 3 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_UINT64", sizeof("TYPE_UINT64")-1, 4 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_INT32", sizeof("TYPE_INT32")-1, 5 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_FIXED64", sizeof("TYPE_FIXED64")-1, 6 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_FIXED32", sizeof("TYPE_FIXED32")-1, 7 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_BOOL", sizeof("TYPE_BOOL")-1, 8 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_STRING", sizeof("TYPE_STRING")-1, 9 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_GROUP", sizeof("TYPE_GROUP")-1, 10 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_MESSAGE", sizeof("TYPE_MESSAGE")-1, 11 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_BYTES", sizeof("TYPE_BYTES")-1, 12 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_UINT32", sizeof("TYPE_UINT32")-1, 13 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_ENUM", sizeof("TYPE_ENUM")-1, 14 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_SFIXED32", sizeof("TYPE_SFIXED32")-1, 15 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_SFIXED64", sizeof("TYPE_SFIXED64")-1, 16 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_SINT32", sizeof("TYPE_SINT32")-1, 17 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "TYPE_SINT64", sizeof("TYPE_SINT64")-1, 18 TSRMLS_CC);
    zend_declare_class_constant_long(protocol_buffers_class_entry, "MAX_FIELD_TYPE", sizeof("MAX_FIELD_TYPE")-1, 19 TSRMLS_CC);
}


zend_module_entry protocolbuffers_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"protocolbuffers",
	NULL,					/* Functions */
	PHP_MINIT(protocolbuffers),	/* MINIT */
	PHP_MSHUTDOWN(protocolbuffers),	/* MSHUTDOWN */
	PHP_RINIT(protocolbuffers),	/* RINIT */
	PHP_RSHUTDOWN(protocolbuffers),		/* RSHUTDOWN */
	PHP_MINFO(protocolbuffers),	/* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
	PHP_PROTOCOLBUFFERS_EXTVER,
#endif
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_PROTOCOLBUFFERS
ZEND_GET_MODULE(protocolbuffers)
#endif
