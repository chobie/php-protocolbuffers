#ifndef PHP_PROTOCOLBUFFERS_DESCRIPTOR_BUILDER_H
#define PHP_PROTOCOLBUFFERS_DESCRIPTOR_BUILDER_H

void php_pb_descriptor_builder_class(TSRMLS_D);
int php_pb_field_descriptor_get_property(HashTable *hash, const char *name, size_t name_len, zval **result TSRMLS_DC);

#endif