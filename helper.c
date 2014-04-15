#include "protocolbuffers.h"
#include "unknown_field_set.h"
#include "unknown_field.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_helper_debug_zval, 0, 0, 1)
	ZEND_ARG_INFO(0, zval)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_helper_write_varint32, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_helper_write_varint64, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_helper_zigzag_encode32, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_helper_zigzag_decode32, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_helper_zigzag_encode64, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_helper_zigzag_decode64, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static void php_protocolbuffers_helper_debug_zval(zval **value TSRMLS_DC)
{
	zval *val = *value;

	php_printf("{\n");
	php_printf("  address: 0x%lx,\n", (unsigned long)val);
	php_printf("  type: %d,\n", val->type);
	php_printf("  is_ref: %d,\n", PZVAL_IS_REF(val));
	php_printf("  refcount: %d,\n", Z_REFCOUNT_PP(value));
	php_printf("  value: {\n");
	php_printf("    lval: %ld,\n", val->value.lval);
	php_printf("    double: %f,\n", val->value.dval);
	if (val->type == 4) {
		php_printf("    ht: {\n");
		php_printf("      address: 0x%lx,\n", (unsigned long)val->value.ht);
		php_printf("      num_of_elements: %d,\n", (unsigned int)val->value.ht->nNumOfElements);
		php_printf("      next_free_elements: %d,\n", (unsigned int)val->value.ht->nNextFreeElement);
		php_printf("    },\n");
	}
	php_printf("    object: {\n");
	php_printf("      handle: 0x%x,\n", val->value.obj.handle);
	php_printf("      handlers: 0x%lx,\n", (unsigned long)val->value.obj.handlers);
	php_printf("    },\n");
	php_printf("  }\n");
	php_printf("}\n");
}

/* {{{ proto bool ProtocolBuffersHelper::debugZval()
*/
PHP_METHOD(protocolbuffers_helper, debugZval)
{
	zval *val = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"z", &val) == FAILURE) {
		return;
	}
	php_protocolbuffers_helper_debug_zval(&val TSRMLS_CC);
}
/* }}} */

/* {{{ proto long ProtocolBuffersHelper::zigzagEncode32()
*/
PHP_METHOD(protocolbuffers_helper, zigzagEncode32)
{
	long val = 0;
	union {
		long l;
		uint32_t v;
	} u;
	uint32_t value = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &val) == FAILURE) {
		return;
	}
	u.l = val;

	value = zigzag_encode32(u.v);
	RETURN_LONG(value);
}
/* }}} */

/* {{{ proto long ProtocolBuffersHelper::zigzagDecode32()
*/
PHP_METHOD(protocolbuffers_helper, zigzagDecode32)
{
	long val = 0;
	union {
		long l;
		int32_t v;
	} u;
	int32_t value = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &val) == FAILURE) {
		return;
	}
	u.l = val;

	value = zigzag_decode32(u.v);
	RETURN_LONG(value);
}
/* }}} */

/* {{{ proto long ProtocolBuffersHelper::zigzagEncode64()
*/
PHP_METHOD(protocolbuffers_helper, zigzagEncode64)
{
	long val = 0;
	union {
		long l;
		uint64_t v;
	} u;
	uint64_t value = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &val) == FAILURE) {
		return;
	}
	u.l = val;

	value = zigzag_encode64(u.v);
	RETURN_LONG(value);
}
/* }}} */

/* {{{ proto long ProtocolBuffersHelper::zigzagDecode64()
*/
PHP_METHOD(protocolbuffers_helper, zigzagDecode64)
{
	long val = 0;
	union {
		long l;
		int64_t v;
	} u;
	int64_t value = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &val) == FAILURE) {
		return;
	}
	u.l = val;

	value = zigzag_decode64(u.v);
	RETURN_LONG(value);
}
/* }}} */


/* {{{ proto string ProtocolBuffersHelper::writeVarint32(long value)
*/
PHP_METHOD(protocolbuffers_helper, writeVarint32)
{
	long val = 0;
	int32_t value = 0;
	uint8_t bytes[kMaxVarint32Bytes];
	int size = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &val) == FAILURE) {
		return;
	}
	if (val > kint32max) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "passed variable exceeds int32 max.");
		return;
	}
	if (val < kint32min) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "passed variable exceeds int32 min.");
		return;
	}

	value = val;
	while (value > 0x7F) {
		bytes[size++] = (value & 0x7F) | 0x80;
		value >>= 7;
	}
	bytes[size++] = value & 0x7F;

	RETURN_STRINGL((char*)bytes, size, 1);
}
/* }}} */

/* {{{ proto string ProtocolBuffersHelper::writeVarint64(long value)
*/
PHP_METHOD(protocolbuffers_helper, writeVarint64)
{
	long val = 0;
	int size = 0;
	int64_t value = 0;
	uint8_t bytes[kMaxVarintBytes];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"l", &val) == FAILURE) {
		return;
	}
	value = val;

	while (value > 0x7F) {
		bytes[size++] = (value & 0x7F) | 0x80;
		value >>= 7;
	}
	bytes[size++] = value & 0x7F;

	RETURN_STRINGL((char*)bytes, size, 1);
}
/* }}} */

static zend_function_entry protocolbuffers_helper_methods[] = {
	PHP_ME(protocolbuffers_helper, writeVarint32, arginfo_protocolbuffers_helper_write_varint32, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(protocolbuffers_helper, writeVarint64, arginfo_protocolbuffers_helper_write_varint64, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(protocolbuffers_helper, zigzagEncode32, arginfo_protocolbuffers_helper_zigzag_encode32, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(protocolbuffers_helper, zigzagDecode32, arginfo_protocolbuffers_helper_zigzag_decode32, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(protocolbuffers_helper, zigzagEncode64, arginfo_protocolbuffers_helper_zigzag_encode64, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(protocolbuffers_helper, zigzagDecode64, arginfo_protocolbuffers_helper_zigzag_decode64, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(protocolbuffers_helper, debugZval, arginfo_protocolbuffers_helper_debug_zval, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_FE_END
};

void php_protocolbuffers_helper_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersHelper", protocolbuffers_helper_methods);
	php_protocol_buffers_helper_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "Helper", php_protocol_buffers_helper_class_entry);
}
