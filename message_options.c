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

#include "protocolbuffers.h"
#include "message_options.h"

int php_protocolbuffers_message_options_init_properties(zval *object TSRMLS_DC)
{
	HashTable *properties = NULL;
	zval *tmp = NULL;

	ALLOC_HASHTABLE(properties);
	zend_hash_init(properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	MAKE_STD_ZVAL(tmp);
	array_init(tmp);
	zend_hash_update(properties, "extensions", sizeof("extensions"), (void **)&tmp, sizeof(zval), NULL);

	zend_merge_properties(object, properties, 1 TSRMLS_CC);

	return 0;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_message_options_get_options, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

/* {{{ proto ProtocolBuffers_MessageOptions::getExtension(string $name)
*/
PHP_METHOD(protocolbuffers_message_options, getExtension)
{
	zval **result = NULL, *options = NULL;
	char *name = {0};
	long name_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"s", &name, &name_len) == FAILURE) {
		return;
	}

	options = zend_read_property(php_protocol_buffers_descriptor_builder_class_entry, getThis(), ZEND_STRS("extensions")-1, 1 TSRMLS_CC);

	if (zend_hash_find(Z_ARRVAL_P(options), name, name_len, (void **)&result) != SUCCESS) {
		if (strcmp(name, "php") == 0) {
			zval *obj;

			MAKE_STD_ZVAL(obj);
			object_init_ex(obj, php_protocol_buffers_php_message_options_class_entry);

			zend_hash_update(Z_ARRVAL_P(options), name, name_len+1, (void **)&obj, sizeof(obj), NULL);
			result = &obj;
		} else {
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "%s extension does not support. now only supports php extension", name);
			return;
		}
	}

	RETURN_ZVAL(*result, 1, 0);
}
/* }}} */

static zend_function_entry php_protocolbuffers_message_options_methods[] = {
	PHP_ME(protocolbuffers_message_options, getExtension, arginfo_protocolbuffers_message_options_get_options, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void php_protocolbuffers_message_options_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersMessageOptions", php_protocolbuffers_message_options_methods);
	php_protocol_buffers_message_options_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	zend_declare_property_null(php_protocol_buffers_message_options_class_entry, ZEND_STRS("extensions")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "MessageOptions", php_protocol_buffers_message_options_class_entry);
}
