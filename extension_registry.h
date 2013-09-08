#ifndef PHP_PROTOCOLBUFFERS_EXTENSION_REGISTRY_H
#define PHP_PROTOCOLBUFFERS_EXTENSION_REGISTRY_H

void php_pb_extension_registry_class(TSRMLS_D);
zval *php_protocolbuffers_extension_registry_get_instance(TSRMLS_D);
int php_protocolbuffers_extension_registry_get_descriptor_by_name(zval *hash, const char* name, size_t name_len, zval **result TSRMLS_DC);
int php_protocolbuffers_extension_registry_get_registry(zval *instance, const char* message_class, size_t message_class_len, zval **result TSRMLS_DC);

#endif