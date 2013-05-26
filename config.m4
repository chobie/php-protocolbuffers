PHP_ARG_ENABLE(protocolbuffers, Whether to enable the "protocolbuffers" extension,
	[ --enable-protocolbuffers 	Enable "protocolbuffers" extension support])

PHP_ARG_ENABLE(protocolbuffers-debug, for protocolbuffers debug support,
    [ --enable-protocolbuffers-debug       Enable enable protocolbuffers debug support], no, no)

if test "$PHP_PROTOCOLBUFFERS_DEBUG" != "no"; then
    CXXFLAGS="$CFLAGS -Wall -g -ggdb -O0 -DPHP_PROTOCOLBUFFERS_DEBUG=1"
    AC_DEFINE(PHP_PROTOCOLBUFFERS_DEBUG, 1, [Enable uv debug support])
fi

if test $PHP_PROTOCOLBUFFERS != "no"; then
    PHP_REQUIRE_CXX()
    PHP_ADD_LIBRARY(stdc++,, PROTOCOLBUFFERS_SHARED_LIBADD)

    PHP_SUBST(PROTOCOLBUFFERS_SHARED_LIBADD)
    PHP_NEW_EXTENSION(protocolbuffers, protocol_buffers.cc, $ext_shared)

    PHP_SUBST([CXXFLAGS])
fi
