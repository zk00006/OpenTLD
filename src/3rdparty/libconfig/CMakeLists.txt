#This are the defines for building a static libconfig for windows
if(WIN32)
	add_definitions(-DLIBCONFIGXX_STATIC -DLIBCONFIG_STATIC -DYY_NO_UNISTD_H -DYY_USE_CONST -D_CRT_SECURE_NO_DEPRECATE -D_STDLIB_H)
endif(WIN32)

add_library(libconfig grammar.c libconfig.c libconfigcpp.cc scanctx.c scanner.c strbuf.c)
