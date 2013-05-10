PHP_ARG_ENABLE(protocolbuffers, Whether to enable the "protocolbuffers" extension,
	[ --enable-protocolbuffers 	Enable "protocolbuffers" extension support])

if test $PHP_PROTOCOLBUFFERS != "no"; then
    PHP_SUBST(PROTOCOLBUFFERS_SHARED_LIBADD)

    PHP_NEW_EXTENSION(protocolbuffers, protocol_buffers.c, $ext_shared)

    CFLAGS=" -Wunused-variable -Wpointer-sign -Wimplicit-function-declaration"
    PHP_SUBST([CFLAGS])
fi
