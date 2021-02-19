
AM_CPPFLAGS = -I@top_srcdir@/include
AM_LDFLAGS = -lgtest

LIB_DIR = @abs_top_builddir@
bin_PROGRAMS = sort handler position
handler_SOURCES = handler.cpp
handler_LDADD = $(LIB_DIR)/lib_path.la \
				$(LIB_DIR)/lib_handler.la \
                $(LIB_DIR)/lib_compare.la

sort_SOURCES = sort.cpp
sort_LDADD =    $(LIB_DIR)/lib_compare.la \
                $(LIB_DIR)/lib_handler.la

position_SOURCES = position.cpp
position_LDADD = $(LIB_DIR)/lib_position.la



TESTS = $(bin_PROGRAMS)
