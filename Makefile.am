
AM_CPPFLAGS = -I@srcdir@/src

base = src/base
basedir = @abs_builddir@/base
base_LTLIBRARIES = libcompare.la libpath.la

    libcompare_la_SOURCES = $(base)/compare.cpp

    libpath_la_SOURCES = $(base)/path.cpp


handler = src/handler
handlerdir = @abs_builddir@/handler
handler_LTLIBRARIES = lib_default_handler.la lib_wxarchive_handler.la lib_handler_factory.la lib_struct_stream.la

    lib_default_handler_la_SOURCES = $(handler)/default_handler.cpp
    lib_default_handler_la_LIBADD = lib_struct_stream.la

    lib_wxarchive_handler_la_SOURCES = $(handler)/wxarchive_handler.cpp
    lib_wxarchive_handler_la_LIBADD = lib_default_handler.la libcompare.la libpath.la

    lib_handler_factory_la_SOURCES = $(handler)/handler_factory.cpp
    lib_handler_factory_la_LIBADD =  lib_default_handler.la lib_wxarchive_handler.la libcompare.la libpath.la

    lib_struct_stream_la_SOURCES = $(handler)/struct_stream.cpp



# make libraries for bitmap
bitmap = src/bitmap
bitmapdir = @abs_builddir@/bitmap
bitmap_LTLIBRARIES = libbitmap.la libsize.la libposition.la libbmp.la

    libbmp_la_SOURCES = $(bitmap)/bmp.cpp
    # make source and link for bitmap
    libbitmap_la_SOURCES = $(bitmap)/bitmap.cpp
    libbitmap_la_LIBADD = libsize.la libposition.la libbmp.la

    libsize_la_SOURCES = $(bitmap)/size.cpp

    libposition_la_SOURCES = $(bitmap)/position.cpp

reader = src/reader
readerdir = @abs_builddir@/reader   
reader_LTLIBRARIES =  lib_load_reader.la lib_window_reader.la lib_zoom_reader.la

    lib_load_reader_la_SOURCES = $(reader)/load_reader.cpp
    lib_load_reader_la_LIBADD = libthread.la 

    lib_zoom_reader_la_SOURCES = $(reader)/zoom_reader.cpp
    lib_zoom_reader_la_LIBADD = libthread.la lib_struct_stream.la

    lib_window_reader_la_SOURCES = $(reader)/window_reader.cpp
    lib_window_reader_la_LIBADD = lib_load_reader.la lib_handler_factory.la libscrolledwindow.la libbitmap.la libbmp.la lib_zoom_reader.la

# make libraries for gui
gui = src/gui
guidir = @abs_builddir@/gui
gui_LTLIBRARIES = libframe.la libpanel.la
    # make source and link for frame
    libframe_la_SOURCES = $(gui)/frame.cpp
    libframe_la_LIBADD = libpanel.la
    # make source and link for panel
    libpanel_la_SOURCES = $(gui)/panel.cpp
    libpanel_la_LIBADD = lib_window_reader.la

window = src/window
windowdir = @abs_builddir@/window
window_LTLIBRARIES = libscrolledwindow.la
    libscrolledwindow_la_SOURCES = $(window)/scrolledwindow.cpp

thread = src/thread
threaddir = @abs_builddir@/thread
thread_LTLIBRARIES = libthread.la
    libthread_la_SOURCES = $(thread)/thread.cpp

bin_PROGRAMS = fmr
fmr_SOURCES = src/main/app.cpp
fmr_LDADD = libframe.la

    
