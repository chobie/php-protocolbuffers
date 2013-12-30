#include "protocolbuffers.h"
#include "enum_descriptor_builder.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_enum_descriptor_builder_add_value, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, force_add)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_protocolbuffers_enum_descriptor_builder_build, 0, 0, 0)
ZEND_END_ARG_INFO()

/* {{{ proto void ProtocolBuffersDescriptorBuilder::__construct()
*/
PHP_METHOD(protocolbuffers_enum_descriptor_builder, __construct)
{
}
/* }}} */

/* {{{ proto void ProtocolBuffersDescriptorBuilder::addValue(ProtocolBuffers\EnumValueDescriptor $value[, bool $force = false])
*/
PHP_METHOD(protocolbuffers_enum_descriptor_builder, addValue)
{
	zval *instance = getThis();
	zval *value;
	zend_bool force = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"O|b", &value, php_protocol_buffers_enum_value_descriptor_class_entry, &force) == FAILURE) {
		return;
	}
}
/* }}} */

/* {{{ proto ProtocolBuffers\EnumDescriptor ProtocolBuffersDescriptorBuilder::build()
*/
PHP_METHOD(protocolbuffers_enum_descriptor_builder, build)
{
	zval *result;

	MAKE_STD_ZVAL(result);
	object_init_ex(result, php_protocol_buffers_enum_descriptor_class_entry);

	RETURN_ZVAL(result, 0, 1);
}
/* }}} */


static zend_function_entry protocolbuffers_enum_descriptor_builder_methods[] = {
	PHP_ME(protocolbuffers_enum_descriptor_builder, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(protocolbuffers_enum_descriptor_builder, addValue,  arginfo_protocolbuffers_enum_descriptor_builder_add_value, ZEND_ACC_PUBLIC)
	PHP_ME(protocolbuffers_enum_descriptor_builder, build,     arginfo_protocolbuffers_enum_descriptor_builder_build, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void php_protocolbuffers_enum_descriptor_builder_class(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "ProtocolBuffersEnumDescriptorBuilder", protocolbuffers_enum_descriptor_builder_methods);
	php_protocol_buffers_enum_descriptor_builder_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(PHP_PROTOCOLBUFFERS_NAMESPACE, "EnumDescriptorBuilder", php_protocol_buffers_enum_descriptor_builder_class_entry);
}
