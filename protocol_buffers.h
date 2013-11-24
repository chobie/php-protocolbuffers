#ifndef __PROTOCOLBUFFERS_H
#define __PROTOCOLBUFFERS_H
void php_protocolbuffers_init(TSRMLS_D);
void messages_dtor(pb_scheme *entry);

static void php_invalid_byte_sequence_exception(TSRMLS_D);
#endif