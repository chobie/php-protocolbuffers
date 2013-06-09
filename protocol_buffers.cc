// Protocol Buffers for PHP - Google's data interchange format
// Copyright 2013 Shuhei Tanuma.  All rights reserved.
//
// https://github.com/chobie/php-protocolbuffers
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "php_protocol_buffers.h"
#include "protocol_buffers.h"

#include "is_utf8.h"
#include "descriptor.h"
#include "field_descriptor.h"
#include "field_options.h"
#include "message.h"
#include "message_options.h"
#include "descriptor_builder.h"

#ifdef ZTS
int pb_globals_id;
#else
pb_globals php_pb_globals;
#endif

static const int kMaxVarintBytes = 10;
static const int kMaxVarint32Bytes = 5;
static const int64_t kLongMax = LONG_MAX;

zend_class_entry *protocol_buffers_class_entry;
zend_class_entry *protocol_buffers_descriptor_class_entry;
zend_class_entry *protocol_buffers_field_descriptor_class_entry;
zend_class_entry *protocol_buffers_field_options_class_entry;
zend_class_entry *protocol_buffers_message_class_entry;
zend_class_entry *protocol_buffers_message_options_class_entry;
zend_class_entry *protocol_buffers_descriptor_builder_class_entry;

zend_class_entry *protocol_buffers_invalid_byte_sequence_class_entry;
zend_class_entry *protocol_buffers_invalid_protocolbuffers_exception_class_entry;
zend_class_entry *protocol_buffers_uninitialized_message_exception_class_entry;

static zend_class_entry *php_pb_get_exception_base(TSRMLS_D)
{
#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 2)
        return zend_exception_get_default();
#else
        return zend_exception_get_default(TSRMLS_C);
#endif
}

static void pb_globals_ctor(pb_globals *pb_globals_p TSRMLS_DC)
{
}

static void pb_globals_dtor(pb_globals *pb_globals_p TSRMLS_DC)
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

static inline const char* ReadVarint32FromArray(const char* buffer, uint* value, const char* buffer_end) {
    // Fast path:  We have enough bytes left in the buffer to guarantee that
    // this read won't cross the end, so we can skip the checks.
    const char* ptr = buffer;
    int b;
    int result;

    if (GOOGLE_PREDICT_TRUE(buffer < buffer_end) && (uint)*ptr < 0x80) {
        *value = (uint)*buffer;
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

static inline const char* ReadVarint64FromArray(const char* buffer, uint64_t* value, const char* buffer_end)
{
    const char* ptr = buffer;
    uint32_t b;

    // Splitting into 32-bit pieces gives better performance on 32-bit
    // processors.
    uint32_t part0 = 0, part1 = 0, part2 = 0;

    b = *(ptr++); part0  = (b & 0x7F)      ; if (!(b & 0x80)) goto done;
    b = *(ptr++); part0 |= (b & 0x7F) <<  7; if (!(b & 0x80)) goto done;
    b = *(ptr++); part0 |= (b & 0x7F) << 14; if (!(b & 0x80)) goto done;
    b = *(ptr++); part0 |= (b & 0x7F) << 21; if (!(b & 0x80)) goto done;
    b = *(ptr++); part1  = (b & 0x7F)      ; if (!(b & 0x80)) goto done;
    b = *(ptr++); part1 |= (b & 0x7F) <<  7; if (!(b & 0x80)) goto done;
    b = *(ptr++); part1 |= (b & 0x7F) << 14; if (!(b & 0x80)) goto done;
    b = *(ptr++); part1 |= (b & 0x7F) << 21; if (!(b & 0x80)) goto done;
    b = *(ptr++); part2  = (b & 0x7F)      ; if (!(b & 0x80)) goto done;
    b = *(ptr++); part2 |= (b & 0x7F) <<  7; if (!(b & 0x80)) goto done;

    // We have overrun the maximum size of a varint (10 bytes).  The data
    // must be corrupt.
    return "";

   done:
    *value = (uint64_t)(part0)         |
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

static void pb_convert_msg(HashTable *proto, const char *klass, int klass_len, pb_scheme **scheme, int *size TSRMLS_DC)
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

            tsize                  = Z_STRLEN_P(tmp)+1;
            ischeme[n].name        = (char*)emalloc(sizeof(char*) * tsize);
            ischeme[n].name_len    = tsize;

            memcpy(ischeme[n].name, Z_STRVAL_P(tmp), tsize);
            ischeme[n].name[tsize] = '\0';

            zend_mangle_property_name(&mangle, &mangle_len, (char*)"*", 1, (char*)ischeme[n].name, ischeme[n].name_len, 0);
            ischeme[n].mangled_name     = mangle;
            ischeme[n].mangled_name_len = mangle_len;

            ischeme[n].repeated = pb_get_lval_from_hash_by_tag(proto, ttag, "repeated", sizeof("repeated") TSRMLS_CC);
            ischeme[n].packed   = pb_get_lval_from_hash_by_tag(proto, ttag, "packable", sizeof("packable") TSRMLS_CC);

            if (ischeme[n].type == TYPE_MESSAGE) {

                pb_get_zval_from_hash_by_tag(proto, ttag, "message", sizeof("message"), &tmp TSRMLS_CC);

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

    if (zend_hash_find(PBG(messages), (char*)klass, klass_len, (void **)&cn) != SUCCESS) {
        zval *ret = NULL;
        HashTable *proto = NULL;


        if (descriptor == NULL) {
            zend_class_entry **ce;

            zend_lookup_class((char*)klass, klass_len, &ce TSRMLS_CC);
            if (zend_call_method(NULL, *ce, NULL, "getdescriptor", strlen("getdescriptor"), &ret, 0, NULL, NULL  TSRMLS_CC)) {
                if (Z_TYPE_P(ret) == IS_ARRAY) {
                    proto = Z_ARRVAL_P(ret);
                } else if (Z_TYPE_P(ret) == IS_OBJECT) {
                    //TODO: needs refactoring.
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
                php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_get_scheme_cointainer failed. %s does not have getDescriptor method", klass);
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

static inline void php_pb_decode_add_value_and_consider_repeated(pb_scheme_container *container, pb_scheme *s, HashTable *hresult, zval *dz)
{
    char *name;
    int name_len;

    if (container->use_single_property < 1) {
        name = s->mangled_name;
        name_len = s->mangled_name_len;
    } else {
        name = s->name;
        name_len = s->name_len;
    }

    if (s->repeated) {


        if (!zend_hash_exists(hresult, name, name_len)) {
            zval *arr;
    
            MAKE_STD_ZVAL(arr);
            array_init(arr);
    
            zend_hash_next_index_insert(Z_ARRVAL_P(arr), (void *)&dz, sizeof(dz), NULL);
            Z_ADDREF_P(dz);
    
            zend_hash_update(hresult, name, name_len, (void **)&arr, sizeof(arr), NULL);
            Z_ADDREF_P(arr);
            zval_ptr_dtor(&arr);
        } else {
            zval **arr2;

            if (zend_hash_find(hresult, name, name_len, (void **)&arr2) == SUCCESS) {
                if (Z_TYPE_PP(arr2) == IS_NULL) {
                    array_init(*arr2);
                }

                zend_hash_next_index_insert(Z_ARRVAL_PP(arr2), (void *)&dz, sizeof(dz), NULL);
                Z_ADDREF_P(dz);
            }
        }
    } else {
        zend_hash_update(hresult, name, name_len, (void **)&dz, sizeof(dz), NULL);
        Z_ADDREF_P(dz);
    }
    
    zval_ptr_dtor(&dz);
}

static const char* pb_decode_message(INTERNAL_FUNCTION_PARAMETERS, const char *data, const char *data_end, pb_scheme_container *container, zval **result)
{
    uint value = 0, tag = 0, wiretype = 0;
    zval *tmp, *dz;
    HashTable *hresult;
    char buffer[512] = {0};

    //hresult = Z_ARRVAL_PP(result);
    hresult = Z_OBJPROP_PP(result);


    while (data < data_end) {
        pb_scheme *s;

        data = ReadVarint32FromArray(data, &value, data_end);

        if (data == NULL) {
            php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_decode_message failed. ReadVarint32FromArray returns NULL.");
            return NULL;
        }

        tag      = (value >> 0x03);
        wiretype = (value & 0x07);

        s = pb_search_scheme_by_tag(container->scheme, container->size, tag);
        if (s == NULL) {
            //php_error_docref(NULL TSRMLS_CC, E_WARNING, "tag %d NOTFOUND. this is bug (currently, unknown field does not support yet)\n", tag);
            return NULL;
        }

        switch (wiretype) {
        case WIRETYPE_VARINT:
        {
            data = ReadVarint32FromArray(data, &value, data_end);
            MAKE_STD_ZVAL(dz);

            if (s->type == TYPE_BOOL) {
                ZVAL_BOOL(dz, value);
            } else if (s->type == TYPE_INT32) {
                ZVAL_LONG(dz, (int32_t)value);
            } else if (s->type == TYPE_SINT32) {
                ZVAL_LONG(dz, (int32_t)zigzag_decode32(value));
            } else {
                ZVAL_LONG(dz, value);
            }

            php_pb_decode_add_value_and_consider_repeated(container, s, hresult, dz);
        }
        break;
        case WIRETYPE_FIXED64:
            if (s->type == TYPE_DOUBLE) {
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

            if (s->type == TYPE_STRING) {
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
                zval *z_arr, *z_obj;
                pb_scheme_container *c_container;
                char *name;
                int name_length;

                pb_get_scheme_container(s->ce->name, s->ce->name_length, &c_container, NULL TSRMLS_CC);

                // TODO: remove needles value
                //MAKE_STD_ZVAL(z_arr);
                //array_init(z_arr);

                MAKE_STD_ZVAL(z_obj);
                object_init_ex(z_obj, s->ce);

                pb_decode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, data, n_buffer_end, c_container, &z_obj);

                //TODO: able to choose single property or else.
                //zend_mangle_property_name(&name, &name_length, (char*)"*", 1, (char*)"_properties", sizeof("_properties"), 0);
                //zend_hash_update(Z_OBJPROP_P(z_obj), name, name_length, (void **)&z_arr, sizeof(zval*), NULL);
                //efree(name);

                // TODO
                //pb_execute_wakeup(z_obj TSRMLS_CC);

                if (s->repeated) {
                    if (!zend_hash_exists(hresult, s->mangled_name, s->mangled_name_len)) {
                        zval *arr;

                        MAKE_STD_ZVAL(arr);
                        array_init(arr);

                        zend_hash_next_index_insert(Z_ARRVAL_P(arr), (void *)&z_obj, sizeof(z_obj), NULL);
                        Z_ADDREF_P(z_obj);

                        zend_hash_update(hresult, s->mangled_name, s->mangled_name_len, (void **)&arr, sizeof(arr), NULL);
                        Z_ADDREF_P(arr);
                        zval_ptr_dtor(&arr);
                    } else {
                        zval **arr2;

                        if (zend_hash_find(hresult, s->mangled_name, s->mangled_name_len, (void **)&arr2) == SUCCESS) {
                            zend_hash_next_index_insert(Z_ARRVAL_PP(arr2), (void *)&z_obj, sizeof(z_obj), NULL);
                            Z_ADDREF_P(z_obj);
                        }
                    }
                } else {
                    zend_hash_update(hresult, s->mangled_name, s->mangled_name_len, (void **)&z_obj, sizeof(z_obj), NULL);
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
                                // PHP_INT_MAX is 0x7fffffff on this platform. cast to double.
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
            if (s->type == TYPE_FLOAT) {
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
                    // PHP_INT_MAX is 0x7fffffff on this platform. cast to double.
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

    //*result = tmp;

    return data;
}



static void pb_serializer_destroy(pb_serializer *serializer)
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


static void pb_serializer_init(pb_serializer **serializer)
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
    int i;

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
//        fprintf(stderr, "out of bounds. write_varint32 expects %d\n", kint32max);
//        return 0;
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
    if (is_packed == 0) {
        pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_FIXED32);
    }

    if (Z_TYPE_PP(element) != IS_LONG) {
        convert_to_long(*element);
    }

    pb_serializer_write32_le(ser, Z_LVAL_PP(element));
}

static void pb_encode_element_fixed64(PB_ENCODE_CALLBACK_PARAMETERS)
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

static void pb_encode_element_uint64(PB_ENCODE_CALLBACK_PARAMETERS)
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

static void pb_encode_element_int32(PB_ENCODE_CALLBACK_PARAMETERS)
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

static void pb_encode_element_string(PB_ENCODE_CALLBACK_PARAMETERS)
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

static void pb_encode_element_msg(PB_ENCODE_CALLBACK_PARAMETERS)
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
    // TODO: add error handling

    pb_encode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, *element, n_container, &n_ser);
    pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_LENGTH_DELIMITED);
    pb_serializer_write_varint32(ser, n_ser->buffer_size);
    pb_serializer_write_chararray(ser, n_ser->buffer, n_ser->buffer_size);

    efree(n_ser->buffer);
    efree(n_ser);
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
    if (Z_TYPE_PP(element) != IS_LONG) {
        convert_to_long(*element);
    }

    if (is_packed == 0) {
        pb_serializer_write_varint32(ser, (scheme->tag << 3) | WIRETYPE_VARINT);
    }

    pb_serializer_write_varint32(ser, Z_LVAL_PP(element));
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

static void pb_encode_element_sfixed64(PB_ENCODE_CALLBACK_PARAMETERS)
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

static void pb_encode_element(INTERNAL_FUNCTION_PARAMETERS, pb_scheme_container *container, HashTable *hash, pb_scheme *scheme, pb_serializer *ser, pb_encode_callback f, int is_packed)
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
                f(INTERNAL_FUNCTION_PARAM_PASSTHRU, tmp, scheme, ser, is_packed);
            }
        }
    }
}

static int pb_encode_message(INTERNAL_FUNCTION_PARAMETERS, zval *klass, pb_scheme_container *container, pb_serializer **serializer)
{
    int i = 0;
    pb_serializer *ser;
    zval **c, *targets = NULL;
    HashTable *hash = NULL;
    pb_scheme *scheme;
    char *name;
    int name_length;

    pb_serializer_init(&ser);

    // TODO: consider interface.
//    pb_execute_sleep(klass, &targets TSRMLS_CC);
//    if (targets != NULL) {
//        // TODO: make hash?
//    }

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

    *serializer = ser;
    return 0;
}


/* {{{ proto mixed ProtocolBuffers::decode($class_name, $bytes)
*/
PHP_METHOD(protocolbuffers, decode)
{
    HashTable *proto = NULL;
    char *klass;
    const char *data, *data_end, *res;
    long klass_len = 0, data_len = 0;
    long buffer_size = 0;
    zval *obj, *z_result, *z_proto = NULL;
    pb_scheme_container *container;
    int err = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "ss|z", &klass, &klass_len, &data, &data_len, &z_proto) == FAILURE) {
        return;
    }

    if (data_len < 1) {
        zend_throw_exception_ex(protocol_buffers_invalid_protocolbuffers_exception_class_entry, 0 TSRMLS_CC, "passed variable seems null");
        return;
    }

    if (z_proto && Z_TYPE_P(z_proto) == IS_ARRAY) {
        proto       = Z_ARRVAL_P(z_proto);
    }

    buffer_size = (long)data + sizeof(data);
    err = pb_get_scheme_container(klass, klass_len, &container, proto TSRMLS_CC);
    if (err) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_get_scheme_container failed. %s does not have getDescriptor method", klass);
        return;
    }

    data_end = data + data_len;

    {
        //zval func;
        zval **pp[1];
        zend_class_entry **ce;
        char *name;
        int name_length;

        MAKE_STD_ZVAL(obj);
        zend_lookup_class(klass, klass_len, &ce TSRMLS_CC);
        //pp[0] = &z_result;

        //ZVAL_STRINGL(&func, "__construct", sizeof("__construct") - 1, 0);

        object_init_ex(obj, *ce);

        //MAKE_STD_ZVAL(z_result);
        //array_init(z_result);
        res = pb_decode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, data, data_end, container, &obj);
        if (res == NULL) {
            zval_ptr_dtor(&obj);
            zend_throw_exception_ex(protocol_buffers_invalid_protocolbuffers_exception_class_entry, 0 TSRMLS_CC, "passed variable contains malformed byte sequence. or it contains unsupported tag");
            return;
        }

        //call_user_function_ex(NULL, &obj, &func, &ret, 1, pp, 0, NULL  TSRMLS_CC);
        //Z_OBJPROP_P(obj)
        //zend_mangle_property_name(char **dest, int *dest_length, const char *src1, int src1_length, const char *src2, int src2_length, int internal)
        //zend_mangle_property_name(&kkey, &klen, "*", 1, "_properties", sizeof("_properties")+1, 0);
        //zend_hash_add(Z_OBJPROP_P(obj), kkey, klen, (void **)&z_result, sizeof(zval*), NULL);

        //zend_hash_find(Z_OBJPROP_P(obj), "_properties", sizeof("_properties")+1, (void **)&h);

//        zend_mangle_property_name(&name, &name_length, (char*)"*", 1, (char*)"_properties", sizeof("_properties"), 0);
//        zend_hash_update(Z_OBJPROP_P(obj), name, name_length, (void **)&z_result, sizeof(zval*), NULL);
//        efree(name);

        //zend_hash_update(Z_OBJPROP_P(obj), kkey, klen, (void **)&z_result, sizeof(zval*), NULL);

        //Z_ADDREF_P(z_result);
        //zval_ptr_dtor(&z_result);

        // TODO:
        //pb_execute_wakeup(obj TSRMLS_CC);
    }

    RETURN_ZVAL(obj, 0, 1);
}
/* }}} */

/* {{{ proto string ProtocolBuffers::encode($class[, array $descriptor])
*/
PHP_METHOD(protocolbuffers, encode)
{
    zval *klass, *z_descriptor = NULL;
    zend_class_entry *ce;
    pb_scheme_container *container;
    HashTable *proto = NULL;
    pb_serializer *ser = NULL;
    int err = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "o|a", &klass, &z_descriptor) == FAILURE) {
        return;
    }

    ce = Z_OBJCE_P(klass);

    if (z_descriptor) {
        proto       = Z_ARRVAL_P(z_descriptor);
    }

    err = pb_get_scheme_container(ce->name, ce->name_length, &container, proto TSRMLS_CC);
    if (err) {
        if (EG(exception)) {
            return;
        } else {
            // TODO: improve displaying error message
            php_error_docref(NULL TSRMLS_CC, E_ERROR, "pb_get_scheme_cointainer failed. %s does not have getDescriptor method", ce->name);
            return;
        }
    }

    if (pb_encode_message(INTERNAL_FUNCTION_PARAM_PASSTHRU, klass, container, &ser)) {
        return;
    }

    if (ser == NULL) {
        RETURN_EMPTY_STRING();
    }

    if (ser->buffer_size > 0) {
        RETVAL_STRINGL((char*)ser->buffer, ser->buffer_size, 1);

        pb_serializer_destroy(ser);
    } else {
        pb_serializer_destroy(ser);
        RETURN_EMPTY_STRING();
    }
}
/* }}} */

static zend_function_entry php_protocolbuffers_methods[] = {
    PHP_ME(protocolbuffers, decode, arginfo_pb_decode, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
    PHP_ME(protocolbuffers, encode, NULL, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


static void php_invalid_byte_sequence_exception(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ProtocolBuffers_InvalidByteSequenceException", 0);
    protocol_buffers_invalid_byte_sequence_class_entry = zend_register_internal_class_ex(&ce, php_pb_get_exception_base(TSRMLS_C), NULL TSRMLS_CC);
}

static void php_protocol_buffers_invalid_exception(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ProtocolBuffers_InvalidProtocolBufferException", 0);
    protocol_buffers_invalid_protocolbuffers_exception_class_entry = zend_register_internal_class_ex(&ce, php_pb_get_exception_base(TSRMLS_C), NULL TSRMLS_CC);

}

static void php_uninitialized_message_exception(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ProtocolBuffers_UninitializedMessage", 0);
    protocol_buffers_uninitialized_message_exception_class_entry = zend_register_internal_class_ex(&ce, php_pb_get_exception_base(TSRMLS_C), NULL TSRMLS_CC);

}

void php_protocolbuffers_init(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ProtocolBuffers", php_protocolbuffers_methods);
    protocol_buffers_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

    php_invalid_byte_sequence_exception(TSRMLS_C);
    php_protocol_buffers_invalid_exception(TSRMLS_C);
    php_uninitialized_message_exception(TSRMLS_C);
    php_pb_descriptor_class(TSRMLS_C);
    php_pb_filed_descriptor_class(TSRMLS_C);
    php_pb_filed_options_class(TSRMLS_C);
    php_pb_message_class(TSRMLS_C);
    php_pb_message_options_class(TSRMLS_C);
    php_pb_descriptor_builder_class(TSRMLS_C);

#define PB_DECLARE_CONST_LONG(name, size, value) \
    zend_declare_class_constant_long(protocol_buffers_class_entry, name, size, value TSRMLS_CC);

    PB_DECLARE_CONST_LONG("WIRETYPE_VARINT",           sizeof("WIRETYPE_VARINT")-1,           0);
    PB_DECLARE_CONST_LONG("WIRETYPE_FIXED64",          sizeof("WIRETYPE_FIXED64")-1,          1);
    PB_DECLARE_CONST_LONG("WIRETYPE_LENGTH_DELIMITED", sizeof("WIRETYPE_LENGTH_DELIMITED")-1, 2);
    PB_DECLARE_CONST_LONG("WIRETYPE_START_GROUP",      sizeof("WIRETYPE_START_GROUP")-1,      3);
    PB_DECLARE_CONST_LONG("WIRETYPE_END_GROUP",        sizeof("WIRETYPE_END_GROUP")-1,        4);
    PB_DECLARE_CONST_LONG("WIRETYPE_FIXED32",          sizeof("WIRETYPE_FIXED32")-1,          5);

    PB_DECLARE_CONST_LONG("TYPE_DOUBLE",               sizeof("TYPE_DOUBLE")-1,               1);
    PB_DECLARE_CONST_LONG("TYPE_FLOAT",                sizeof("TYPE_FLOAT")-1,                2);
    PB_DECLARE_CONST_LONG("TYPE_INT64",                sizeof("TYPE_INT64")-1,                3);
    PB_DECLARE_CONST_LONG("TYPE_UINT64",               sizeof("TYPE_UINT64")-1,               4);
    PB_DECLARE_CONST_LONG("TYPE_INT32",                sizeof("TYPE_INT32")-1,                5);
    PB_DECLARE_CONST_LONG("TYPE_FIXED64",              sizeof("TYPE_FIXED64")-1,              6);
    PB_DECLARE_CONST_LONG("TYPE_FIXED32",              sizeof("TYPE_FIXED32")-1,              7);
    PB_DECLARE_CONST_LONG("TYPE_BOOL",                 sizeof("TYPE_BOOL")-1,                 8);
    PB_DECLARE_CONST_LONG("TYPE_STRING",               sizeof("TYPE_STRING")-1,               9);
    PB_DECLARE_CONST_LONG("TYPE_GROUP",                sizeof("TYPE_GROUP")-1,               10);
    PB_DECLARE_CONST_LONG("TYPE_MESSAGE",              sizeof("TYPE_MESSAGE")-1,             11);
    PB_DECLARE_CONST_LONG("TYPE_BYTES",                sizeof("TYPE_BYTES")-1,               12);
    PB_DECLARE_CONST_LONG("TYPE_UINT32",               sizeof("TYPE_UINT32")-1,              13);
    PB_DECLARE_CONST_LONG("TYPE_ENUM",                 sizeof("TYPE_ENUM")-1,                14);
    PB_DECLARE_CONST_LONG("TYPE_SFIXED32",             sizeof("TYPE_SFIXED32")-1,            15);
    PB_DECLARE_CONST_LONG("TYPE_SFIXED64",             sizeof("TYPE_SFIXED64")-1,            16);
    PB_DECLARE_CONST_LONG("TYPE_SINT32",               sizeof("TYPE_SINT32")-1,              17);
    PB_DECLARE_CONST_LONG("TYPE_SINT64",               sizeof("TYPE_SINT64")-1,              18);
    PB_DECLARE_CONST_LONG("MAX_FIELD_TYPE",            sizeof("MAX_FIELD_TYPE")-1,           19);

#undef PB_DECLARE_CONST_LONG

}

PHP_MINFO_FUNCTION(protocolbuffers)
{

    php_printf("PHP Protocol Buffers Extension\n");

    php_info_print_table_start();
    php_info_print_table_header(2, "Protocol Buffers Support", "enabled");
    php_info_print_table_row(2, "Version", PHP_PROTOCOLBUFFERS_EXTVER);
    php_info_print_table_end();
}

PHP_MINIT_FUNCTION(protocolbuffers)
{

#ifdef ZTS
    ts_allocate_id(&pb_globals_id, sizeof(pb_globals), (ts_allocate_ctor) pb_globals_ctor, (ts_allocate_dtor) pb_globals_dtor);
#else
    pb_globals_ctor(&php_pb_globals TSRMLS_CC);
#endif

    php_protocolbuffers_init(TSRMLS_C);
    return SUCCESS;
}

PHP_RINIT_FUNCTION(protocolbuffers)
{
    PBG(messages) = NULL;

    if (!PBG(messages)) {
        ALLOC_HASHTABLE(PBG(messages));
        zend_hash_init(PBG(messages), 0, NULL, NULL, 0);
    }

    return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(protocolbuffers)
{
#ifdef ZTS
        ts_free_id(pb_globals_id);
#else
        pb_globals_dtor(&php_pb_globals TSRMLS_CC);
#endif

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(protocolbuffers)
{
    if (PBG(messages)) {
        zend_try {
            int i = 0;
            HashPosition pos;
            pb_scheme_container **element;

            for(zend_hash_internal_pointer_reset_ex(PBG(messages), &pos);
                            zend_hash_get_current_data_ex(PBG(messages), (void **)&element, &pos) == SUCCESS;
                            zend_hash_move_forward_ex(PBG(messages), &pos)
            ) {

                for (i = 0; i < (*element)->size; i++) {
                    if ((*element)->scheme[i].name != NULL) {
                        efree((*element)->scheme[i].name);
                    }
                    if ((*element)->scheme[i].mangled_name != NULL) {
                        efree((*element)->scheme[i].mangled_name);
                    }
                }

                if ((*element)->single_property_name != NULL) {
                    efree((*element)->single_property_name);
                }

                efree((*element)->scheme);
                efree(*element);
            }

            zend_hash_destroy(PBG(messages));
            FREE_HASHTABLE(PBG(messages));
            PBG(messages) = NULL;
        }
        zend_end_try();
    }

    return SUCCESS;
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
