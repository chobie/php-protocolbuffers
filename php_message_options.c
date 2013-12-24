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
#include "php_message_options.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_php_message_options_set_use_single_property, 0, 0, 1)
	ZEND_ARG_INFO(0, use)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_php_message_options_set_single_property_name, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_php_message_options_set_process_unkown_fields, 0, 0, 1)
	ZEND_ARG_INFO(0, use)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pb_php_message_options_set_use_wakeup_and_sleep, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

static void php_protocolbuffers_message_options_set_value(INTERNAL_FUNCTION_PARAMETERS, char *name, size_t length, int type)
{
	zval *value = NULL, **target = NULL, *instance = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"z", &value) == FAILURE) {
		return;
	}

	if (zend_hash_find(Z_OBJPROP_P(instance), name, length, (void **)&target) == SUCCESS) {
		if (type == IS_BOOL) {
			if (Z_TYPE_P(value) != IS_BOOL) {
				convert_to_boolean(value);
			}

			ZVAL_BOOL(*target, Z_BVAL_P(value));
		} else if (type == IS_STRING) {
			zval *tmp;

			MAKE_STD_ZVAL(tmp);
			ZVAL_STRING(tmp, Z_STRVAL_P(value), 1);
			zend_hash_update(Z_OBJPROP_P(instance), name, length, (void **)&tmp, sizeof(zval*), NULL);
		}
	}
}

/* {{{ proto void ProtocolBuffersPHPMessageOptions::setUseWakeupAndSleep(boolean $use)
*/
PHP_METHOD(protocolbuffers_php_message_options, setUseWakeupAndSleep)
{
	php_protocolbuffers_message_options_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_STRS("use_wakeup_and_sleep"), IS_BOOL);
}
/* }}} */

/* {{{ proto void ProtocolBuffersPHPMessageOptions::setUseSingleProperty(boolean $use)
*/
PHP_METHOD(protocolbuffers_php_message_options, setUseSingleProperty)
{
	php_protocolbuffers_message_options_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_STRS("use_single_property"), IS_BOOL);
}
/* }}} */

/* {{{ proto void ProtocolBuffersPHPMessageOptions::setSinglePropertyName(string $name)
*/
PHP_METHOD(protocolbuffers_php_message_options, setSinglePropertyName)
{
	php_protocolbuffers_message_options_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_STRS("single_property_name"), IS_STRING);
}
/* }}} */

/* {{{ proto void ProtocolBuffersPHPMessageOptions::setProcessUnknownFields(boolean $use)
*/
PHP_METHOD(protocolbuffers_php_message_options, setProcessUnknownFields)
{
	php_protocolbuffers_message_options_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_STRS("process_unknown_fields"), IS_BOOL);
}
/* }}} */

static zend_function_entry php_protocolbuffers_php_message_options_methods[] = {
	PHP_ME(protocolbuffers_php_message_options, setUseSingleProperty, arginfo_pb_php_message_options_set_use_single_property, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_php_message_options, setUseWakeupAndSleep, arginfo_pb_php_message_options_set_use_wakeup_and_sleep, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_php_message_options, setSinglePropertyName, arginfo_pb_php_message_options_set_single_property_name, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_php_message_options, setProcessUnknownFields, arginfo_pb_php_message_options_set_process_unkown_fields, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void php_protocolbuffers_php_message_options_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersPHPMessageOptions", php_protocolbuffers_php_message_options_methods);
	protocol_buffers_php_message_options_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_bool(protocol_buffers_php_message_options_class_entry, ZEND_STRS("use_single_property")-1, 0, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(protocol_buffers_php_message_options_class_entry, ZEND_STRS("single_property_name")-1, "_properties", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_bool(protocol_buffers_php_message_options_class_entry, ZEND_STRS("process_unknown_fields")-1, 0, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_bool(protocol_buffers_php_message_options_class_entry, ZEND_STRS("use_wakeup_and_sleep")-1, 0, ZEND_ACC_PUBLIC TSRMLS_CC);

	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "PHPMessageOptions", protocol_buffers_php_message_options_class_entry);
}
