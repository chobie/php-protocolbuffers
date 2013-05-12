PHP_ARG_ENABLE(protocolbuffers, Whether to enable the "protocolbuffers" extension,
	[ --enable-protocolbuffers 	Enable "protocolbuffers" extension support])

if test $PHP_PROTOCOLBUFFERS != "no"; then
    PHP_REQUIRE_CXX()
    PHP_ADD_LIBRARY(stdc++,, XHP_SHARED_LIBADD)

    PHP_SUBST(PROTOCOLBUFFERS_SHARED_LIBADD)
    PHP_NEW_EXTENSION(protocolbuffers, protocol_buffers.cc, $ext_shared)

    CXXFLAGS=" -Wunused-variable -Wpointer-sign -Wimplicit-function-declaration -g -O0"
    PHP_SUBST([CXXFLAGS])
fi
