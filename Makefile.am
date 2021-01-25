
AM_CPPFLAGS = -I@srcdir@/include

SUBDIRS = . test

common = src/common
commondir = @abs_builddir@/common
common_LTLIBRARIES = lib_compare.la lib_path.la

    lib_compare_la_SOURCES = $(common)/compare.cpp

    lib_path_la_SOURCES = $(common)/path.cpp


handler = src/handler
handlerdir = @abs_builddir@/handler
handler_LTLIBRARIES = lib_handler.la
lib_handler_la_SOURCES = $(handler)/default_handler.cpp \
						 $(handler)/wxarchive_handler.cpp \
						 $(handler)/handler_factory.cpp \
						 $(handler)/struct_stream.cpp \
						 $(handler)/std_handler.cpp
lib_handler_la_LIBADD = lib_path.la \
						lib_compare.la

# make libraries for bitmap
bitmap = src/bitmap
bitmapdir = @abs_builddir@/bitmap
bitmap_LTLIBRARIES = lib_bitmap.la
lib_bitmap_la_SOURCES = $(bitmap)/bmp.cpp \
						$(bitmap)/bitmap.cpp \
						$(bitmap)/size.cpp \
						$(bitmap)/image_util.cpp \
						$(bitmap)/position.cpp

reader = src/reader
readerdir = @abs_builddir@/reader
reader_LTLIBRARIES =  lib_reader.la
lib_reader_la_SOURCES = $(reader)/window_reader.cpp \
						$(reader)/controller_reader.cpp \
						$(reader)/load_reader.cpp \
						$(reader)/zoom_reader.cpp

lib_reader_la_LIBADD = lib_bitmap.la \
					   lib_handler.la \
					   lib_window.la \
					   lib_thread.la

explorer = src/explorer
explorerdir = @abs_builddir@/explorer
explorer_LTLIBRARIES = lib_explorer.la
lib_explorer_la_SOURCES = $(explorer)/window_explorer.cpp \
						  $(explorer)/controller_explorer.cpp \
						  $(explorer)/image_window_explorer.cpp

lib_explorer_la_LIBADD = lib_handler.la \
						 lib_thread.la \
						 lib_window.la

# make libraries for gui
gui = src/gui
guidir = @abs_builddir@/gui
gui_LTLIBRARIES = lib_frame.la
lib_frame_la_SOURCES = $(gui)/frame.cpp \
					   $(gui)/panel.cpp

lib_frame_la_LIBADD = lib_explorer.la \
					  lib_reader.la


window = src/window
windowdir = @abs_builddir@/window
window_LTLIBRARIES = lib_window.la
lib_window_la_SOURCES = $(window)/scrolledwindow.cpp \
						$(window)/flex_grid_window.cpp \
						$(window)/flex_grid_cell_window.cpp

thread = src/thread
threaddir = @abs_builddir@/thread
thread_LTLIBRARIES = lib_thread.la
lib_thread_la_SOURCES = $(thread)/thread.cpp \
												$(thread)/load_image.cpp \
												$(thread)/load_image_controller.cpp \
												$(thread)/find_handler.cpp \
												$(thread)/find_handler_controller.cpp

fmr = src/main
bin_PROGRAMS = fmr
fmr_SOURCES = $(fmr)/app.cpp
fmr_LDADD = lib_frame.la

