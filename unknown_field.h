#ifndef PHP_PROTOCOLBUFFERS_UNKNOWN_FIELD_H
#define PHP_PROTOCOLBUFFERS_UNKNOWN_FIELD_H

void php_pb_unknown_field_class(TSRMLS_D);

void php_pb_unknown_field_set_number(zval *instance, int number TSRMLS_DC);

void php_pb_unknown_field_set_type(zval *instance, int type TSRMLS_DC);

void php_pb_unknown_field_properties_init(zval *object TSRMLS_DC);

#endif