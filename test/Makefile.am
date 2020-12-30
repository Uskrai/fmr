
AM_CPPFLAGS = -I@srcdir@/test
AM_LDFLAGS = -lgtest

LIB_DIR = ..
bin_PROGRAMS = handler
handler_SOURCES = handler.cpp
handler_LDADD = $(LIB_DIR)/libpath.la \
                $(LIB_DIR)/lib_struct_stream.la \
                $(LIB_DIR)/lib_std_handler.la


