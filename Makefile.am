
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
						$(bitmap)/rescaler.cpp \
						$(bitmap)/bitmap_ctrl.cpp \
						$(bitmap)/bitmap_page_ctrl.cpp \
						$(bitmap)/image_checker.cpp \
						$(bitmap)/bitmap_draw.cpp
lib_bitmap_la_LIBADD = lib_position.la

loaderdir = @abs_builddir@/loader
loader_LTLIBRARIES = lib_loader.la
loader = src/loader
lib_loader_la_SOURCES = $(loader)/loader.cpp \
												$(loader)/page.cpp \
												$(loader)/rescale.cpp \
												$(loader)/container.cpp \
												$(loader)/rescale_container.cpp
lib_loader_la_LIBADD = lib_queue.la \
											 lib_bitmap.la \
											 lib_thread.la


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
											 lib_thread.la \
											 lib_loader.la

explorer = src/explorer
explorerdir = @abs_builddir@/explorer
explorer_LTLIBRARIES = lib_explorer.la
lib_explorer_la_SOURCES = $(explorer)/controller.cpp \
													$(explorer)/image_cell_explorer.cpp \
													$(explorer)/string_draw.cpp \
													$(explorer)/string_rect.cpp

lib_explorer_la_LIBADD = lib_handler.la \
												 lib_thread.la \
												 lib_window.la \
												 lib_loader.la

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
lib_queue_la_LIBADD = lib_handler.la

file_handler = src/file_handler
file_handlerdir = @abs_builddir@/file_handler
file_handler_LTLIBRARIES = lib_file_handler.la
file_handler_fs = src/file_handler/filesystem
file_handler_filesystem = $(file_handler_fs)/handler.cpp \
													$(file_handler_fs)/input.cpp \
													$(file_handler_fs)/output.cpp \
													$(file_handler_fs)/read_stream.cpp 

file_handler_wxarch = src/file_handler/wx_archive
file_handler_wx_archive = $(file_handler_wxarch)/handler.cpp \
													$(file_handler_wxarch)/input.cpp \
													$(file_handler_wxarch)/read_stream.cpp \
													$(file_handler_wxarch)/output.cpp

lib_file_handler_la_SOURCES = $(file_handler)/memory_stream.cpp \
															$(file_handler)/factory.cpp \
															$(file_handler)/virtual_write/map.cpp \
															$(file_handler_filesystem) \
															$(file_handler_wx_archive)
lib_file_handler_la_LIBADD = lib_path.la \
														 lib_comparer.la

compare = src/compare
comparedir = @abs_builddir@/compare
compare_LTLIBRARIES = lib_comparer.la
lib_comparer_la_SOURCES = $(compare)/natural.cpp



fmr = src/main
bin_PROGRAMS = fmr
fmr_SOURCES = $(fmr)/app.cpp
fmr_LDADD = lib_frame.la

