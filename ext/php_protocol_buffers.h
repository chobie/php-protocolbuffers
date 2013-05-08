#ifndef PHP_PROTOCOLBUFFERS_H

#define PHP_PROTOCOLBUFFERS_H

#define PHP_PROTOCOLBUFFERS_EXTNAME "protocolbuffers"
#define PHP_PROTOCOLBUFFERS_EXTVER "0.1"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "zend_interfaces.h"

/* Define the entry point symbol
 * Zend will use when loading this module
 */
extern zend_module_entry protocolbuffers_module_entry;
#define phpext_protocolbuffers_ptr &protocolbuffers_module_entry;

//extern zend_class_entry *protocolbuffers_parser_class_entry;

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
