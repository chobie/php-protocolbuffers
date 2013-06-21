#ifndef PHP_PROTOCOLBUFFERS_UNKNOWN_FIELD_SET_H
#define PHP_PROTOCOLBUFFERS_UNKNOWN_FIELD_SET_H

void php_pb_unknown_field_set_class(TSRMLS_D);

void php_pb_unknown_field_set_add_field(INTERNAL_FUNCTION_PARAMETERS, zval *instance, int number, char *name, int name_len, zval *field);

int php_pb_unknown_field_get_field(INTERNAL_FUNCTION_PARAMETERS, zval *instance, int number, char *name, int name_len, php_protocolbuffers_unknown_field **f);

void php_pb_unknown_field_clear(INTERNAL_FUNCTION_PARAMETERS, zval *instance);

#endif