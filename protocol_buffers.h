
void php_protocolbuffers_init(TSRMLS_D);
void messages_dtor(pb_scheme *entry);

//static void pb_execute_wakeup(zval *obj TSRMLS_DC);
//static void pb_execute_sleep(zval *obj, zval **retval TSRMLS_DC);

static void php_invalid_byte_sequence_exception(TSRMLS_D);
