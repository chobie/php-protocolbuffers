
void php_protocolbuffers_init(TSRMLS_D);
void messages_dtor(pb_scheme *entry);

static int pb_get_tag_name(HashTable *proto, ulong tag, zval **result TSRMLS_DC);
static int pb_get_msg_name(HashTable *proto, ulong tag, zval **result TSRMLS_DC);
static int pb_tag_is_string(HashTable *proto, ulong tag TSRMLS_DC);
static int pb_tag_is_message(HashTable *proto, ulong tag TSRMLS_DC);
static int pb_tag_type(HashTable *proto, ulong tag TSRMLS_DC);
static int pb_tag_wiretype(HashTable *proto, ulong tag TSRMLS_DC);
static inline pb_scheme *pb_search_scheme_by_tag(pb_scheme* scheme, uint scheme_size, uint tag);
static void pb_convert_msg(HashTable *proto, const char *klass, int klass_len, pb_scheme **scheme, int *size TSRMLS_DC);
static int pb_get_scheme_container(const char *klass, size_t klass_len, pb_scheme_container **result, HashTable *descriptor TSRMLS_DC);
static const char* pb_decode_message(INTERNAL_FUNCTION_PARAMETERS, const char *data, const char *data_end, pb_scheme_container *container, zval **result);

static void pb_execute_wakeup(zval *obj TSRMLS_DC);
static void pb_execute_sleep(zval *obj, zval **retval TSRMLS_DC);

static void php_invalid_byte_sequence_exception(TSRMLS_D);
static void pb_serializer_destroy(pb_serializer *serilizer);

static void pb_serializer_init(pb_serializer **serializer);
static int pb_serializer_resize(pb_serializer *serializer, size_t size);
static int pb_serializer_write8(pb_serializer *serializer, unsigned int value);
static int pb_serializer_write16(pb_serializer *serializer, unsigned int value);
static int pb_serializer_write32(pb_serializer *serializer, unsigned int value);
static int pb_serializer_write32_le(pb_serializer *serializer, unsigned int value);
static int pb_serializer_write64_le(pb_serializer *serializer, uint64_t value);
static int pb_serializer_write64_le2(pb_serializer *serializer, int64_t value);
static int pb_serializer_write64(pb_serializer *serializer, uint64_t value);
static int pb_serializer_write_varint32(pb_serializer *serializer, uint32_t value);
static int pb_serializer_write_varint64(pb_serializer *serializer, uint64_t value);
static int pb_serializer_write_chararray(pb_serializer *serializer, unsigned char *string, size_t len);

static int pb_encode_message(INTERNAL_FUNCTION_PARAMETERS, zval *klass, pb_scheme_container *container, pb_serializer **serializer);

typedef void (*pb_encode_callback)(INTERNAL_FUNCTION_PARAMETERS, zval **element, pb_scheme *scheme, pb_serializer *ser);
