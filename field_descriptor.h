#ifndef PHP_PROTOCOLBUFFERS_FIELD_DESCRIPTOR_H
#define PHP_PROTOCOLBUFFERS_FIELD_DESCRIPTOR_H

void php_pb_filed_descriptor_class(TSRMLS_D);
int php_pb_field_descriptor_get_name(zval *instance, char **retval, int *len TSRMLS_DC);

#endif