#ifndef PHP_PROTOCOLBUFFERS_H

#define PHP_PROTOCOLBUFFERS_H

#define PHP_PROTOCOLBUFFERS_EXTNAME "protocolbuffers"
#define PHP_PROTOCOLBUFFERS_EXTVER "0.1"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "ext/standard/php_smart_str.h"

#include "strutil.h"

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


/* Define the entry point symbol
 * Zend will use when loading this module
 */
extern zend_module_entry protocolbuffers_module_entry;
#define phpext_protocolbuffers_ptr &protocolbuffers_module_entry;

extern zend_class_entry *protocol_buffers_class_entry;

#ifndef GOOGLE_PREDICT_TRUE
#ifdef __GNUC__
// Provided at least since GCC 3.0.
#define GOOGLE_PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#else
#define GOOGLE_PREDICT_TRUE
#endif
#endif


typedef struct {
    HashTable *messages;
} pb_globals;

typedef struct pb_scheme
{
	int tag;
	char *name;
	int name_len;
	int wiretype;
	int type;
	int flags;
	int scheme_type;
	char *message;
	char *enum_msg;
	zend_class_entry *ce;
} pb_scheme;

typedef struct {
    pb_scheme *scheme;
    int size;
} pb_scheme_container;

#ifdef ZTS
#define PBG(v) TSRMG(pb_globals_id, pb_globals *, v)
PHPAPI extern int pb_globals_id;
#else
#define PBG(v) (globals.v)
PHPAPI extern pb_globals pb_globals;
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

#endif /* PHP_PROTOCOLBUFFERS_H */
