
AM_CPPFLAGS = -I@srcdir@/include

SUBDIRS = . test
#
# AM_CPPFLAGS += $(BOOST_CPPFLAGS)
# AM_LDFLAGS = $(BOOST_LDFLAGS) \
						 # $(BOOST_FILESYSTEM_LIBS)

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
						 $(handler)/std_handler.cpp \
						 $(handler)/stream_util.cpp

lib_handler_la_LIBADD = lib_path.la \
						lib_compare.la \
						$(BOOST_FILESYSTEM_LDFLAGS)

# make libraries for bitmap
bitmap = src/bitmap
bitmapdir = @abs_builddir@/bitmap
bitmap_LTLIBRARIES = lib_bitmap.la
lib_bitmap_la_SOURCES = $(bitmap)/bmp.cpp \
						$(bitmap)/image_util.cpp \
						$(bitmap)/loader.cpp \
						$(bitmap)/rescaler.cpp \
						$(bitmap)/bitmap_ctrl.cpp \
						$(bitmap)/bitmap_page_ctrl.cpp \
						$(bitmap)/page_loader.cpp \
						$(bitmap)/rescale_loader.cpp \
						$(bitmap)/image_checker.cpp
lib_bitmap_la_LIBADD = lib_position.la

position = src/position
positiondir = @abs_builddir@/position
position_LTLIBRARIES = lib_position.la
lib_position_la_SOURCES = $(position)/box_ctrl.cpp \
													$(position)/grid_ctrl.cpp

reader = src/reader
readerdir = @abs_builddir@/reader
reader_LTLIBRARIES =  lib_reader.la
lib_reader_la_SOURCES = $(reader)/controller.cpp \
												$(reader)/scroll_controller.cpp \
												$(reader)/decorator_ctrl.cpp \
												$(reader)/page_indicator.cpp

lib_reader_la_LIBADD = lib_bitmap.la \
						 lib_handler.la \
						 lib_window.la \
						 lib_thread.la

explorer = src/explorer
explorerdir = @abs_builddir@/explorer
explorer_LTLIBRARIES = lib_explorer.la
lib_explorer_la_SOURCES = $(explorer)/window_explorer.cpp \
													$(explorer)/image_window_explorer.cpp \
													$(explorer)/controller.cpp \
													$(explorer)/image_cell_explorer.cpp

lib_explorer_la_LIBADD = lib_handler.la \
						 lib_thread.la \
						 lib_window.la

gui = src/gui
guidir = @abs_builddir@/gui
gui_LTLIBRARIES = lib_frame.la
lib_frame_la_SOURCES = $(gui)/frame.cpp \
						 $(gui)/panel.cpp \
						 $(gui)/menu_item_toggler.cpp \
						 $(gui)/accelerator_table.cpp

lib_frame_la_LIBADD = lib_explorer.la \
						lib_reader.la


window = src/window
windowdir = @abs_builddir@/window
window_LTLIBRARIES = lib_window.la
lib_window_la_SOURCES = $(window)/scrolledwindow.cpp \
												$(window)/flex_grid_window.cpp \
												$(window)/flex_grid_cell_window.cpp \
												$(window)/scrolled_image.cpp \
												$(window)/grid_window.cpp \
												$(window)/grid_item.cpp \
												$(window)/menu_bar.cpp \
												$(window)/decorator.cpp
lib_window_la_LIBADD = lib_position.la

thread = src/thread
threaddir = @abs_builddir@/thread
thread_LTLIBRARIES = lib_thread.la
lib_thread_la_SOURCES = $(thread)/thread.cpp \
												$(thread)/find_handler_controller.cpp \
												$(thread)/controller_factory.cpp

lib_thread_la_LIBADD = lib_queue.la

queue = src/queue
queuedir = @abs_builddir@/queue
queue_LTLIBRARIES = lib_queue.la
lib_queue_la_SOURCES = $(queue)/load_image.cpp \
											 $(queue)/find_handler.cpp \
											 $(queue)/rescale.cpp  \
											 $(queue)/factory.cpp


fmr = src/main
bin_PROGRAMS = fmr
fmr_SOURCES = $(fmr)/app.cpp
fmr_LDADD = lib_frame.la

