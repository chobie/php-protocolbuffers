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

#ifndef PHP_PROTOCOLBUFFERS_CORE_H
#define PHP_PROTOCOLBUFFERS_CORE_H

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

int php_protocolbuffers_read_protected_property(zval *instance, char *name, size_t name_len, zval **result TSRMLS_DC);

int php_protocolbuffers_set_protected_property(zval *instance, char *name, size_t name_len, zval *value TSRMLS_DC);

#define PB_ENCODE_CALLBACK_PARAMETERS INTERNAL_FUNCTION_PARAMETERS, zval **element, php_protocolbuffers_scheme *scheme, php_protocolbuffers_serializer *ser, int is_packed
typedef void (*php_protocolbuffers_encode_callback)(PB_ENCODE_CALLBACK_PARAMETERS);

int php_protocolbuffers_encode(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *ce, zval *klass);

int php_protocolbuffers_decode(INTERNAL_FUNCTION_PARAMETERS, const char *data, int data_len, const char *klass, int klass_len);

void php_protocolbuffers_process_unknown_field(INTERNAL_FUNCTION_PARAMETERS, php_protocolbuffers_scheme_container *container, HashTable *hresult, zval *dz, int tag, int wiretype, int64_t value);

void php_protocolbuffers_process_unknown_field_bytes(INTERNAL_FUNCTION_PARAMETERS, php_protocolbuffers_scheme_container *container, HashTable *hresult, int tag, int wiretype, uint8_t *bytes, int length);

int php_protocolbuffers_properties_init(zval *object, zend_class_entry *ce TSRMLS_DC);

char *php_protocolbuffers_get_default_single_property_name();

int php_protocolbuffers_get_default_single_property_name_len();

char *php_protocolbuffers_get_default_unknown_property_name();

int php_protocolbuffers_get_default_unknown_property_name_len();

void php_protocolbuffers_scheme_container_init(php_protocolbuffers_scheme_container *container);

int php_protocolbuffers_get_scheme_container(const char *klass, size_t klass_len, php_protocolbuffers_scheme_container **result TSRMLS_DC);

void php_protocolbuffers_format_string(zval *result, pbf *payload TSRMLS_DC);

void php_protocolbuffers_execute_wakeup(zval *obj, php_protocolbuffers_scheme_container *container TSRMLS_DC);

void php_protocolbuffers_execute_sleep(zval *obj, php_protocolbuffers_scheme_container *container TSRMLS_DC);

#include "core_inl.h"

#endif