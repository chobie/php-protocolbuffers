#ifndef PHP_PROTOCOLBUFFERS_H

#define PHP_PROTOCOLBUFFERS_H

#define PHP_PROTOCOLBUFFERS_EXTNAME "protocolbuffers"
#define PHP_PROTOCOLBUFFERS_EXTVER "0.1"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "zend.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "ext/standard/php_smart_str.h"
#include "ext/spl/spl_exceptions.h"

#include <stdint.h>

#ifndef PHP_PROTOCOLBUFFERS_DEBUG
#define PHP_PROTOCOLBUFFERS_DEBUG 0
#endif


#ifdef _MSC_VER
  #if defined(_M_IX86) && \
      !defined(PROTOBUF_DISABLE_LITTLE_ENDIAN_OPT_FOR_TEST)
    #define PROTOBUF_LITTLE_ENDIAN 1
  #endif
  #if _MSC_VER >= 1300
    // If MSVC has "/RTCc" set, it will complain about truncating casts at
    // runtime.  This file contains some intentional truncating casts.
    #pragma runtime_checks("c", off)
  #endif
#else
  #include <sys/param.h>   // __BYTE_ORDER
  #if defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN && \
      !defined(PROTOBUF_DISABLE_LITTLE_ENDIAN_OPT_FOR_TEST)
    #define PROTOBUF_LITTLE_ENDIAN 1
  #endif
#endif


// long long macros to be used because gcc and vc++ use different suffixes,
// and different size specifiers in format strings
#undef GOOGLE_LONGLONG
#undef GOOGLE_ULONGLONG
#undef GOOGLE_LL_FORMAT

#ifdef _MSC_VER
#define GOOGLE_LONGLONG(x) x##I64
#define GOOGLE_ULONGLONG(x) x##UI64
#define GOOGLE_LL_FORMAT "I64"  // As in printf("%I64d", ...)
#else
#define GOOGLE_LONGLONG(x) x##LL
#define GOOGLE_ULONGLONG(x) x##ULL
#define GOOGLE_LL_FORMAT "ll"  // As in "%lld". Note that "q" is poor form also.
#endif

static const int32_t kint32max = 0x7FFFFFFF;
static const int32_t kint32min = -kint32max - 1;
static const int64_t kint64max = GOOGLE_LONGLONG(0x7FFFFFFFFFFFFFFF);
static const int64_t kint64min = -kint64max - 1;
static const uint32_t kuint32max = 0xFFFFFFFFu;
static const uint64_t kuint64max = GOOGLE_ULONGLONG(0xFFFFFFFFFFFFFFFF);

/* Define the entry point symbol
 * Zend will use when loading this module
 */
extern zend_module_entry protocolbuffers_module_entry;
#define phpext_protocolbuffers_ptr &protocolbuffers_module_entry

extern zend_class_entry *protocol_buffers_class_entry;
extern zend_class_entry *protocol_buffers_descriptor_class_entry;
extern zend_class_entry *protocol_buffers_field_descriptor_class_entry;
extern zend_class_entry *protocol_buffers_field_options_class_entry;
extern zend_class_entry *protocol_buffers_message_class_entry;
extern zend_class_entry *protocol_buffers_message_options_class_entry;
extern zend_class_entry *protocol_buffers_descriptor_builder_class_entry;

extern zend_class_entry *protocol_buffers_invalid_byte_sequence_class_entry;
extern zend_class_entry *protocol_buffers_invalid_protocolbuffers_exception_class_entry;
extern zend_class_entry *protocol_buffers_uninitialized_message_exception_class_entry;

#ifndef GOOGLE_PREDICT_TRUE
#ifdef __GNUC__
// Provided at least since GCC 3.0.
#define GOOGLE_PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#else
#define GOOGLE_PREDICT_TRUE
#endif
#endif

typedef struct pb_globals{
    HashTable *messages;
} pb_globals;

typedef struct pb_scheme
{
    int tag;
    char *name;
    int name_len;
    int type;
    int flags;
    int scheme_type;
    int repeated;
    int packed;
    char *message;
    char *enum_msg;
    zend_class_entry *ce;
} pb_scheme;

typedef struct {
    char *name;
    int name_len;
    char *filename;
    int filename_len;
    int is_extendable;
    //int extension_range_min;
    pb_scheme *scheme;
    int size;
} pb_scheme_container;

#ifdef ZTS
#define PBG(v) TSRMG(pb_globals_id, pb_globals *, v)
PHPAPI extern int pb_globals_id;
#else
#define PBG(v) (php_pb_globals.v)
PHPAPI extern pb_globals php_pb_globals;
#endif


enum WireType {
    WIRETYPE_VARINT           = 0,
    WIRETYPE_FIXED64          = 1,
    WIRETYPE_LENGTH_DELIMITED = 2,
    WIRETYPE_START_GROUP      = 3,
    WIRETYPE_END_GROUP        = 4,
    WIRETYPE_FIXED32          = 5,
};

// Lite alternative to FieldDescriptor::Type.  Must be kept in sync.
enum FieldType {
    TYPE_DOUBLE         = 1,
    TYPE_FLOAT          = 2,
    TYPE_INT64          = 3,
    TYPE_UINT64         = 4,
    TYPE_INT32          = 5,
    TYPE_FIXED64        = 6,
    TYPE_FIXED32        = 7,
    TYPE_BOOL           = 8,
    TYPE_STRING         = 9,
    TYPE_GROUP          = 10,
    TYPE_MESSAGE        = 11,
    TYPE_BYTES          = 12,
    TYPE_UINT32         = 13,
    TYPE_ENUM           = 14,
    TYPE_SFIXED32       = 15,
    TYPE_SFIXED64       = 16,
    TYPE_SINT32         = 17,
    TYPE_SINT64         = 18,
    MAX_FIELD_TYPE      = 18,
};

typedef struct pb_serializer
{
    uint8_t *buffer;
    size_t buffer_size;
    size_t buffer_capacity;
    size_t buffer_offset;
} pb_serializer;


#endif /* PHP_PROTOCOLBUFFERS_H */
