
AM_CPPFLAGS = -I@top_srcdir@/include
AM_LDFLAGS = -lgtest

LIB_DIR = @abs_top_builddir@
bin_PROGRAMS = sort position file_handler

sort_SOURCES = sort.cpp
sort_LDADD = $(LIB_DIR)/lib_comparer.la

position_SOURCES = position.cpp
position_LDADD = $(LIB_DIR)/lib_position.la

file_handler_SOURCES = file_handler.cpp
file_handler_LDADD = $(LIB_DIR)/lib_file_handler.la



TESTS = $(bin_PROGRAMS)
