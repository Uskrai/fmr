
AM_CPPFLAGS = -I@top_srcdir@/include
AM_LDFLAGS = -lgtest

LIB_DIR = @abs_top_builddir@
bin_PROGRAMS = sort handler
handler_SOURCES = handler.cpp
handler_LDADD = $(LIB_DIR)/lib_path.la \
                $(LIB_DIR)/lib_struct_stream.la \
                $(LIB_DIR)/lib_std_handler.la \
                $(LIB_DIR)/lib_default_handler.la \
                $(LIB_DIR)/lib_wxarchive_handler.la \
                $(LIB_DIR)/lib_compare.la

sort_SOURCES = sort.cpp
sort_LDADD =    $(LIB_DIR)/lib_compare.la \
                $(LIB_DIR)/lib_struct_stream.la



TESTS = $(bin_PROGRAMS)
