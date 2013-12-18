/*
 * Protocol Buffers for PHP
 * Copyright 2013 Shuhei Tanuma.  All rights reserved.
 *
 * https://github.com/chobie/php-protocolbuffers
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PHP_PROTOCOLBUFFERS_SERIALIZER_H
#define PHP_PROTOCOLBUFFERS_SERIALIZER_H

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

#endif