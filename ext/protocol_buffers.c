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


/* {{{ proto class pb_decode(array proto, string class, string data)
*/
PHP_FUNCTION(pb_decode)
{
    HashTable *proto;
    char *class, *data;
    long class_len = 0, data_len = 0;
    int i = 0, offset = 0;
    char bit;
    uint value = 0;
    uint tag   = 0;
    uint wiretype = 0;
    int buffer_size = 0;
    char buffer[512] = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"ass",&proto, &class, &class_len, &data, &data_len) == FAILURE) {
		return;
	}

    buffer_size = data + sizeof(data);
    // what is the better way to detect end of buffer?
    while ((int)data < buffer_size) {
        bit = *data;

        if (bit & 0x80) {
            data++;
            continue;
        } else {
            data = ReadVarint32FromArray(data, &value);

            tag      = (value >> 0x03);
            wiretype = (value & 0x07);

            fprintf(stderr, "[tag: %d, wiretype: %d (%d)]\n", tag, wiretype, value);

            switch (wiretype) {
            case WIRETYPE_VARINT:
                data = ReadVarint32FromArray(data, &value);
                fprintf(stderr, "varint:value: %d\n", value);
            break;
            case WIRETYPE_FIXED64:
                data += 8;
            break;
            case WIRETYPE_LENGTH_DELIMITED:
                data = ReadVarint32FromArray(data, &value);

                // TODO: タイプごとに分ける

                if (value < 512) {
                    memcpy(buffer, data, value);
                    buffer[value+1] = '\0';

                    fprintf(stderr, "length: %d\n", value);
                    fprintf(stderr, "string: %s\n", data);
                } else {
                    char *sub_buffer;
                    sub_buffer = emalloc(value+1);
                    memcpy(sub_buffer, data, value);
                    sub_buffer[value+1] = '\0';

                    fprintf(stderr, "length: %d\n", value);
                    fprintf(stderr, "string: %s\n", data);
                }
                data += value;

            break;
            case WIRETYPE_FIXED32: {
                float a;
                // TODO: タイプ毎にわける
                //a = *(float *)data;
                memcpy(&a, data, 4);
                fprintf(stdout, "fixed32: %10f\n", a);
                data += 4;
            }
            break;
            }
            //data++;
        }
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
