
AM_CPPFLAGS = -I@top_srcdir@/src
AM_LDFLAGS = -lgtest

LIB_DIR = ..
check_PROGRAMS = handler
handler_SOURCES = handler.cpp
handler_LDADD = $(LIB_DIR)/libpath.la \
                $(LIB_DIR)/lib_struct_stream.la \
                $(LIB_DIR)/lib_std_handler.la



TESTS = $(check_PROGRAMS)