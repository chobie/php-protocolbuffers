#include "protocolbuffers.h"
#include "enum.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_enum_is_valid, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()


/* {{{ proto void ProtocolBuffersEnum::__construct()
*/
PHP_METHOD(protocolbuffers_enum, __construct)
{
}
/* }}} */

/* {{{ proto bool ProtocolBuffersEnum::isValid(long $value)
*/
PHP_METHOD(protocolbuffers_enum, isValid)
{
#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 3)
	zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "ProtocolBuffersEnum::isValid can't work under PHP 5.3. please consider upgrading your PHP");
	return;
#else
	zval *value;
	zval *result, *result2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"z", &value) == FAILURE) {
		return;
	}

	if (zend_call_method_with_0_params(NULL, EG(called_scope), NULL, "getenumdescriptor", &result)) {
		if (zend_call_method_with_1_params(&result, Z_OBJCE_P(result), NULL, "isvalid", &result2, value)) {
			if (Z_BVAL_P(result2)) {
				RETVAL_TRUE;
			} else {
				RETVAL_FALSE;
			}
			zval_ptr_dtor(&result2);
		}
		zval_ptr_dtor(&result);
	}
#endif
}
/* }}} */


static zend_function_entry protocolbuffers_enum_methods[] = {
	PHP_ME(protocolbuffers_enum, __construct, NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(protocolbuffers_enum, isValid, arginfo_protocolbuffers_enum_is_valid, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_FE_END
};

void php_protocolbuffers_enum_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersEnum", protocolbuffers_enum_methods);
	php_protocol_buffers_enum_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "Enum", php_protocol_buffers_enum_class_entry);
	zend_class_implements(php_protocol_buffers_enum_class_entry TSRMLS_CC, 1, php_protocol_buffers_enum_describable_class_entry);

	php_protocol_buffers_enum_class_entry->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

}
