
AM_CPPFLAGS = -I@srcdir@/include

SUBDIRS = . test

common = src/common
commondir = @abs_builddir@/common
common_LTLIBRARIES = lib_compare.la lib_path.la

    lib_compare_la_SOURCES = $(common)/compare.cpp

    lib_path_la_SOURCES = $(common)/path.cpp


handler = src/handler
handlerdir = @abs_builddir@/handler
handler_LTLIBRARIES = lib_default_handler.la lib_wxarchive_handler.la lib_handler_factory.la lib_struct_stream.la lib_std_handler.la

    lib_default_handler_la_SOURCES = $(handler)/default_handler.cpp
    lib_default_handler_la_LIBADD = lib_struct_stream.la

    lib_wxarchive_handler_la_SOURCES = $(handler)/wxarchive_handler.cpp
    lib_wxarchive_handler_la_LIBADD = lib_default_handler.la lib_compare.la lib_path.la

    lib_handler_factory_la_SOURCES = $(handler)/handler_factory.cpp
    lib_handler_factory_la_LIBADD =  lib_default_handler.la lib_wxarchive_handler.la lib_compare.la lib_path.la

    lib_struct_stream_la_SOURCES = $(handler)/struct_stream.cpp

    lib_std_handler_la_SOURCES = $(handler)/std_handler.cpp
    lib_std_handler_la_LIBADD = lib_struct_stream.la lib_path.la


# make libraries for bitmap
bitmap = src/bitmap
bitmapdir = @abs_builddir@/bitmap
bitmap_LTLIBRARIES = libbitmap.la libsize.la libposition.la libbmp.la lib_image_util.la

    libbmp_la_SOURCES = $(bitmap)/bmp.cpp
    # make source and link for bitmap
    libbitmap_la_SOURCES = $(bitmap)/bitmap.cpp
    libbitmap_la_LIBADD = libsize.la libposition.la libbmp.la

    libsize_la_SOURCES = $(bitmap)/size.cpp

    lib_image_util_la_SOURCES = $(bitmap)/image_util.cpp
    lib_image_util_la_LIBADD = lib_struct_stream.la

    libposition_la_SOURCES = $(bitmap)/position.cpp

reader = src/reader
readerdir = @abs_builddir@/reader   
reader_LTLIBRARIES =  lib_load_reader.la lib_window_reader.la lib_zoom_reader.la lib_controller_reader.la

    lib_load_reader_la_SOURCES = $(reader)/load_reader.cpp
    lib_load_reader_la_LIBADD = libthread.la lib_image_util.la

    lib_zoom_reader_la_SOURCES = $(reader)/zoom_reader.cpp
    lib_zoom_reader_la_LIBADD = libthread.la lib_struct_stream.la

    lib_controller_reader_la_SOURCES = $(reader)/controller_reader.cpp
    lib_controller_reader_la_LIBADD = lib_load_reader.la lib_handler_factory.la libbitmap.la lib_zoom_reader.la libthread.la

    lib_window_reader_la_SOURCES = $(reader)/window_reader.cpp
    lib_window_reader_la_LIBADD = lib_load_reader.la lib_handler_factory.la libscrolledwindow.la libbitmap.la libbmp.la lib_zoom_reader.la lib_controller_reader.la libthread.la

explorer = src/explorer
explorerdir = @abs_builddir@/explorer
explorer_LTLIBRARIES = lib_window_explorer.la lib_controller_explorer.la lib_image_window_explorer.la lib_find_explorer.la lib_load_explorer.la

    lib_window_explorer_la_SOURCES = $(explorer)/window_explorer.cpp
    lib_window_explorer_la_LIBADD = lib_controller_explorer.la lib_image_window_explorer.la lib_find_explorer.la lib_load_explorer.la lib_flex_grid_window.la

    lib_controller_explorer_la_SOURCES = $(explorer)/controller_explorer.cpp
    lib_controller_explorer_la_LIBADD = lib_find_explorer.la

    lib_image_window_explorer_la_SOURCES = $(explorer)/image_window_explorer.cpp
    lib_image_window_explorer_la_LIBADD = lib_path.la

    lib_find_explorer_la_SOURCES = $(explorer)/find_explorer.cpp
    lib_find_explorer_la_LIBADD = libthread.la

    lib_load_explorer_la_SOURCES = $(explorer)/load_explorer.cpp
    lib_load_explorer_la_LIBADD = libthread.la lib_image_util.la

# make libraries for gui
gui = src/gui
guidir = @abs_builddir@/gui
gui_LTLIBRARIES = libframe.la libpanel.la
    # make source and link for frame
    libframe_la_SOURCES = $(gui)/frame.cpp
    libframe_la_LIBADD = libpanel.la
    # make source and link for panel
    libpanel_la_SOURCES = $(gui)/panel.cpp
    libpanel_la_LIBADD = lib_window_reader.la lib_window_explorer.la

window = src/window
windowdir = @abs_builddir@/window
window_LTLIBRARIES = libscrolledwindow.la lib_flex_grid_window.la lib_flex_grid_cell_window.la
    libscrolledwindow_la_SOURCES = $(window)/scrolledwindow.cpp

    lib_flex_grid_window_la_SOURCES = $(window)/flex_grid_window.cpp
    lib_flex_grid_window_la_LIBADD = lib_flex_grid_cell_window.la

    lib_flex_grid_cell_window_la_SOURCES = $(window)/flex_grid_cell_window.cpp

thread = src/thread
threaddir = @abs_builddir@/thread
thread_LTLIBRARIES = libthread.la
    libthread_la_SOURCES = $(thread)/thread.cpp

bin_PROGRAMS = fmr
fmr_SOURCES = src/main/app.cpp
fmr_LDADD = libframe.la

    
