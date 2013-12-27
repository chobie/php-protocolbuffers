/*
 * Protocol Buffers for PHP
 * Copyright 2013 Shuhei Tanuma.  All rights reserved.
 *
 * https://github.com/chobie/php-protocolbuffers
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PHP_PROTOCOLBUFFERS_PROTOCOL_BUFFERS_H
#define PHP_PROTOCOLBUFFERS_PROTOCOL_BUFFERS_H

#include "php_protocolbuffers.h"

#if defined(__osf__)
#include <inttypes.h>
#elif !defined(_MSC_VER)
#include <stdint.h>
#endif

#if defined(_MSC_VER) && !defined(strcasecmp)
#define strcasecmp stricmp
#endif

#if defined(_MSC_VER) && !defined(strncasecmp)
#define strncasecmp strnicmp
#endif

#if defined(_MSC_VER) && !defined(strtoull)
#define strtoull _strtoui64
#endif

#if defined(_MSC_VER) && !defined(atoll)
#define atoll _atoi64
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

#ifndef PHP_PROTOCOLBUFFERS_DEBUG
#define PHP_PROTOCOLBUFFERS_DEBUG 0
#endif

#define PHP_PROTOCOLBUFFERS_NAMESPACE "ProtocolBuffers"

#if PHP_VERSION_ID < 50300
/* NOTE: do nothing */
#define PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(ns, name, ce)
#else
/* NOTE: zend_register_ns_class_alias had TSRMLS bug under 5.4. we have to define it for compatibility */
#define PHP_PROTOCOLBUFFERS_REGISTER_NS_CLASS_ALIAS(ns, name, ce) \
	zend_register_class_alias_ex(ZEND_NS_NAME(ns, name), sizeof(ZEND_NS_NAME(ns, name))-1, ce TSRMLS_CC)
#endif

#if PHP_VERSION_ID < 50300
#define PHP_PROTOCOLBUFFERS_EXCEPTION_ERROR(exception) zend_exception_error(exception TSRMLS_CC)
#else
#define PHP_PROTOCOLBUFFERS_EXCEPTION_ERROR(exception) zend_exception_error(exception, E_ERROR TSRMLS_CC)
#endif

#if PHP_VERSION_ID < 50300
#define PHP_FE_END            { NULL, NULL, NULL, 0, 0 }
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

static const int32_t ktagmax   = 0x1FFFFFFF;
static const int32_t kint32max = 0x7FFFFFFF;
static const int32_t kint32min = -0x7FFFFFFF - 1;
static const int64_t kint64max = GOOGLE_LONGLONG(0x7FFFFFFFFFFFFFFF);
static const int64_t kint64min = -GOOGLE_LONGLONG(0x7FFFFFFFFFFFFFFF) - 1;
static const uint32_t kuint32max = 0xFFFFFFFFu;
static const uint64_t kuint64max = GOOGLE_ULONGLONG(0xFFFFFFFFFFFFFFFF);

#ifdef _MSC_VER
#define kMaxVarintBytes 10
#define kMaxVarint32Bytes 5
#else
static const int kMaxVarintBytes = 10;
static const int kMaxVarint32Bytes = 5;
#endif

static const int64_t kLongMax = LONG_MAX;

#define PHP_PROTOCOL_BUFFERS_LONG_SIGN_MASK (1L << (8*sizeof(long)-1))

#ifndef GOOGLE_PREDICT_TRUE
#ifdef __GNUC__
// Provided at least since GCC 3.0.
#define GOOGLE_PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#else
#define GOOGLE_PREDICT_TRUE
#endif
#endif

typedef struct php_protocolbuffers_scheme
{
	int tag;
	char *name;
	int name_len;
	ulong name_h;
	char *original_name;
	int original_name_len;
	char *mangled_name;
	int mangled_name_len;
	ulong mangled_name_h;
	int magic_type;
	/* TODO: use flags */
	int type;
	int flags;
	int required;
	int optional;
	int scheme_type;
	int repeated;
	int packed;
	int skip;
	int is_extension;
	char *message;
	char *enum_msg;
	zval *default_value;
	zend_class_entry *ce;
} php_protocolbuffers_scheme;

typedef struct {
	int begin;
	int end;
} php_protocolbuffers_extension_range;

typedef struct {
	char *name;
	int name_len;
	char *filename;
	int filename_len;
	int is_extendable;
	php_protocolbuffers_scheme *scheme;
	int use_single_property;
	char *orig_single_property_name;
	int orig_single_property_name_len;
	char *single_property_name;
	int single_property_name_len;
	ulong single_property_h;
	int size;
	int process_unknown_fields;
	int use_wakeup_and_sleep;
	int extension_cnt;
	php_protocolbuffers_extension_range *extensions;
} php_protocolbuffers_scheme_container;


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

typedef struct php_protocolbuffers_serializer
{
	uint8_t *buffer;
	size_t buffer_size;
	size_t buffer_capacity;
	size_t buffer_offset;
} php_protocolbuffers_serializer;

typedef struct{
	zend_object zo;
	char *name;
	size_t name_len;
	int free_container;
	php_protocolbuffers_scheme_container *container;
} php_protocolbuffers_descriptor;

typedef struct{
	zend_object zo;
	int max;
	int offset;
} php_protocolbuffers_message;

typedef struct{
	zend_object zo;
	int max;
	int offset;
} php_protocolbuffers_unknown_field_set;

typedef struct{
	zend_object zo;
	HashTable *registry;
} php_protocolbuffers_extension_registry;

typedef union {
	uint64_t varint;
	struct {
		uint8_t *val;
		size_t len;
	} buffer;
} unknown_value;

typedef struct{
	zend_object zo;
	int number;
	int type;
	HashTable *ht;
} php_protocolbuffers_unknown_field;


#  if ZEND_MODULE_API_NO >= 20100525
#  define PHP_PROTOCOLBUFFERS_STD_CREATE_OBJECT(STRUCT_NAME) \
	STRUCT_NAME *object;\
	\
	object = (STRUCT_NAME*)ecalloc(1, sizeof(*object));\
	zend_object_std_init(&object->zo, ce TSRMLS_CC);\
	object_properties_init(&object->zo, ce);\
	\
	retval.handle = zend_objects_store_put(object,\
		(zend_objects_store_dtor_t)zend_objects_destroy_object,\
		(zend_objects_free_object_storage_t) STRUCT_NAME##_free_storage ,\
	NULL TSRMLS_CC);\
	retval.handlers = zend_get_std_object_handlers();
#  else
#  define PHP_PROTOCOLBUFFERS_STD_CREATE_OBJECT(STRUCT_NAME) \
	STRUCT_NAME *object;\
	zval *tmp = NULL;\
	\
	object = (STRUCT_NAME*)ecalloc(1, sizeof(*object));\
	zend_object_std_init(&object->zo, ce TSRMLS_CC);\
	zend_hash_copy(object->zo.properties, &ce->default_properties, (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *)); \
	\
	retval.handle = zend_objects_store_put(object,\
		(zend_objects_store_dtor_t)zend_objects_destroy_object,\
		(zend_objects_free_object_storage_t) STRUCT_NAME##_free_storage ,\
	NULL TSRMLS_CC);\
	retval.handlers = zend_get_std_object_handlers();
#  endif

#define PHP_PROTOCOLBUFFERS_GET_OBJECT(STRUCT_NAME, OBJECT) (STRUCT_NAME *)zend_object_store_get_object(OBJECT TSRMLS_CC);

#if PHP_VERSION_ID < 50300
# ifndef Z_ADDREF_P
#  define Z_ADDREF_P(pz)   (pz)->refcount++
#  define Z_ADDREF_PP(ppz) Z_ADDREF_P(*(ppz))
#  define Z_ADDREF(z)      Z_ADDREF_P(&(z))
# endif
# ifndef Z_SET_REFCOUNT_P
#  define Z_SET_REFCOUNT_P(pz,rc) (pz)->refcount = (rc)
# endif
# ifndef Z_REFCOUNT_P
#  define Z_REFCOUNT_P(pz) ((pz)->refcount)
# endif
# ifndef Z_REFCOUNT_PP
#  define Z_REFCOUNT_PP(pz) ((*pz)->refcount)
# endif
#endif

ZEND_BEGIN_MODULE_GLOBALS(protocolbuffers)
	HashTable *messages;
	HashTable *classes;
	zval *extension_registry;
	long strict_mode;
	int validate_string;
ZEND_END_MODULE_GLOBALS(protocolbuffers)

#include "core.h"

#endif