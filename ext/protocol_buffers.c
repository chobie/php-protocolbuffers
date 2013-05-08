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

static const int kMaxVarintBytes = 10;
static const int kMaxVarint32Bytes = 5;

//zend_class_entry *protocolbuffers_class_entry;

void php_protocolbuffers_init(TSRMLS_D);

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, proto)
	ZEND_ARG_INFO(0, class)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_read_varint32, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

PHP_MINFO_FUNCTION(protocolbuffers)
{
}

PHP_MINIT_FUNCTION(protocolbuffers)
{
	php_protocolbuffers_init(TSRMLS_C);
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(protocolbuffers)
{
	return SUCCESS;
}


void php_protocolbuffers_init(TSRMLS_D)
{
}


const char* ReadVarint32FromArray(const char* buffer, uint* value) {
  // Fast path:  We have enough bytes left in the buffer to guarantee that
  // this read won't cross the end, so we can skip the checks.
  const char* ptr = buffer;
  int b;
  int result;

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


/* {{{ proto class pb_decode(array proto, string class, string data)
*/
PHP_FUNCTION(pb_decode)
{
    HashTable *proto, *hresult;
    char *class, *data;
    long class_len = 0, data_len = 0;
    int i = 0, offset = 0;
    char bit;
    uint value = 0, tag = 0, wiretype = 0;
    long buffer_size = 0;
    char buffer[512] = {0};
    zval *z_class, *z_result, *z_proto;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"ass", &z_proto, &class, &class_len, &data, &data_len) == FAILURE) {
		return;
	}

    MAKE_STD_ZVAL(z_result);
    array_init(z_result);

    hresult     = Z_ARRVAL_P(z_result);
    proto       = Z_ARRVAL_P(z_proto);
    buffer_size = data + sizeof(data);

    // what is the better way to detect end of buffer?
    while ((long)data < buffer_size) {
        bit = *data;

        if (bit & 0x80) {
            data++;
            continue;
        } else {
            data = ReadVarint32FromArray(data, &value);

            tag      = (value >> 0x03);
            wiretype = (value & 0x07);

            //fprintf(stderr, "[tag: %d, wiretype: %d (%d)]\n", tag, wiretype, value);

            switch (wiretype) {
            case WIRETYPE_VARINT:
            {
                zval *t;
                data = ReadVarint32FromArray(data, &value);

                if (pb_get_tag_name(proto, tag, &t TSRMLS_CC)) {
                    zval *dz;

                    MAKE_STD_ZVAL(dz);
                    ZVAL_LONG(dz, value);

                    if (Z_TYPE_P(t) != IS_STRING) {
                            convert_to_string(t);
                    }

                    zend_hash_add(hresult, Z_STRVAL_P(t), Z_STRLEN_P(t)+1, (void **)&dz, sizeof(dz), NULL);
                    Z_ADDREF_P(dz);

                    zval_ptr_dtor(&dz);
                }
            }
            break;
            case WIRETYPE_FIXED64:
                // TODO: implement this
                data += 8;
            break;
            case WIRETYPE_LENGTH_DELIMITED:
                data = ReadVarint32FromArray(data, &value);
                char key_buf[256] = {0};
                zval *t;

                if (pb_tag_is_string(proto, tag TSRMLS_CC)) {
                    if (value < 512) {
                        memcpy(buffer, data, value);
                        buffer[value+1] = '\0';
                    } else {
                        char *sub_buffer;
                        sub_buffer = emalloc(value+1);
                        memcpy(sub_buffer, data, value);
                        sub_buffer[value+1] = '\0';
                    }

                    if (pb_get_tag_name(proto, tag, &t TSRMLS_CC)) {
                        zval *dz;

                        MAKE_STD_ZVAL(dz);
                        ZVAL_STRING(dz, data, 1);

                        if (Z_TYPE_P(t) != IS_STRING) {
                                convert_to_string(t);
                        }

                        zend_hash_add(hresult, Z_STRVAL_P(t), Z_STRLEN_P(t)+1, (void **)&dz, sizeof(dz), NULL);
                        Z_ADDREF_P(dz);

                        zval_ptr_dtor(&dz);
                    }

                } else if (pb_tag_is_message(proto, tag TSRMLS_CC)) {

                    if (value < 512) {
                        memcpy(buffer, data, value);
                        buffer[value+1] = '\0';
                    } else {
                        char *sub_buffer;

                        sub_buffer = emalloc(value+1);
                        memcpy(sub_buffer, data, value);
                        sub_buffer[value+1] = '\0';
                    }
                }

                data += value;
            break;
            case WIRETYPE_FIXED32: {
                float a;
                zval *t;

                memcpy(&a, data, 4);

                if (pb_get_tag_name(proto, tag, &t TSRMLS_CC)) {
                    zval *dz;

                    MAKE_STD_ZVAL(dz);
                    ZVAL_DOUBLE(dz, a);

                    if (Z_TYPE_P(t) != IS_STRING) {
                            convert_to_string(t);
                    }

                    zend_hash_add(hresult, Z_STRVAL_P(t), Z_STRLEN_P(t)+1, (void **)&dz, sizeof(dz), NULL);
                    Z_ADDREF_P(dz);
                    zval_ptr_dtor(&dz);
                }

                data += 4;
            }
            break;
            }
        }
    }

    {
        zval *func, *ret, *obj, *params;
        zend_class_entry **ce;
        HashTable *h;
        zval **pp[1];

        MAKE_STD_ZVAL(func);
        MAKE_STD_ZVAL(ret);
        MAKE_STD_ZVAL(obj);

        pp[0] = z_result;
        zend_lookup_class(class, class_len, &ce TSRMLS_CC);

        ZVAL_STRING(func, "__construct", 1);

        object_init_ex(obj, *ce);
        call_user_function(NULL, &obj, func, ret, 1, pp TSRMLS_CC);

        zval_ptr_dtor(&func);
        zval_ptr_dtor(&ret);
        zval_ptr_dtor(&z_result);

        RETURN_ZVAL(obj, 0, 1);
    }
}
/* }}} */


/* {{{ proto int pb_read_varint32(binary value)
*/
PHP_FUNCTION(pb_read_varint32)
{
	char *value = NULL;
	long value_len = 0;
	const uint* ptr;
	uint b;
	uint result;
	int i = 0;
	zval *z_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"s",&value, &value_len) == FAILURE) {
		return;
	}

	MAKE_STD_ZVAL(z_result);
	ZVAL_NULL(z_result);
	ptr = value;

	b = *(ptr++); result  = (b & 0x7F)      ; if (!(b & 0x80)) goto done;
	b = *(ptr++); result |= (b & 0x7F) <<  7; if (!(b & 0x80)) goto done;
	b = *(ptr++); result |= (b & 0x7F) << 14; if (!(b & 0x80)) goto done;
	b = *(ptr++); result |= (b & 0x7F) << 21; if (!(b & 0x80)) goto done;
	b = *(ptr++); result |=  b         << 28; if (!(b & 0x80)) goto done;

	// If the input is larger than 32 bits, we still need to read it all
	// and discard the high-order bits.
	for (i = 0; i < kMaxVarintBytes - kMaxVarint32Bytes; i++) {
	b = *(ptr++); if (!(b & 0x80)) goto done;
	}

	// We have overrun the maximum size of a varint (10 bytes).  Assume
	// the data is corrupt.
	RETURN_ZVAL(z_result, 0, 1);

	done:
	*value = result;

	ZVAL_LONG(z_result, result);
	RETURN_ZVAL(z_result, 0, 1);
}
/* }}} */


static zend_function_entry pb_functions[] = {
	PHP_FE(pb_decode,                     arginfo_pb_decode)
	PHP_FE(pb_read_varint32,              arginfo_pb_read_varint32)
	{NULL, NULL, NULL}
};

zend_module_entry protocolbuffers_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"protocolbuffers",
	pb_functions,					/* Functions */
	PHP_MINIT(protocolbuffers),	/* MINIT */
	NULL,					/* MSHUTDOWN */
	NULL,					/* RINIT */
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
