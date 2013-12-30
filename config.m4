PHP_ARG_ENABLE(protocolbuffers, Whether to enable the "protocolbuffers" extension,
    [ --enable-protocolbuffers  Enable "protocolbuffers" extension support])

PHP_ARG_ENABLE(protocolbuffers-debug, for protocolbuffers debug support,
    [ --enable-protocolbuffers-debug       Enable enable protocolbuffers debug support], no, no)

if test $PHP_PROTOCOLBUFFERS != "no"; then
    if test "$PHP_PROTOCOLBUFFERS_DEBUG" != "no"; then
        CFLAGS="$CFLAGS -Wall -g -ggdb -O0 -DPHP_PROTOCOLBUFFERS_DEBUG=1"
        AC_DEFINE(PHP_PROTOCOLBUFFERS_DEBUG, 1, [Enable protocolbuffers debug support])
    else
        //CFLAGS="$CFLAGS -Wall -g -ggdb -O0 -Wunused-variable -Wpointer-sign -Wimplicit-function-declaration -Winline -Wunused-macros -Wredundant-decls -Wstrict-aliasing=2 -Wswitch-enum -Wdeclaration-after-statement"
    fi

    PHP_PROTOBUF_SRCS="protocolbuffers.c descriptor.c descriptor_builder.c field_descriptor.c message.c message_options.c enum.c enum_descriptor.c enum_value_descriptor.c enum_descriptor_builder.c php_message_options.c unknown_field.c unknown_field_set.c helper.c extension_registry.c core.c serializer.c"

    PHP_SUBST(PROTOCOLBUFFERS_SHARED_LIBADD)
    PHP_NEW_EXTENSION(protocolbuffers, $PHP_PROTOBUF_SRCS, $ext_shared)
    PHP_ADD_EXTENSION_DEP(protocolbuffers, spl, true)

    PHP_SUBST([CFLAGS])
fi
