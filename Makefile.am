
AM_CPPFLAGS = -I@srcdir@/src

base = src/base
basedir = @abs_builddir@/base
base_LTLIBRARIES = libcompare.la libpath.la

    libcompare_la_SOURCES = $(base)/compare.cpp

    libpath_la_SOURCES = $(base)/path.cpp


handler = src/handler
handlerdir = @abs_builddir@/handler
handler_LTLIBRARIES = libdefaulthandler.la libarchivehandler.la libhandlerfactory.la

    libdefaulthandler_la_SOURCES = $(handler)/defaulthandler.cpp

    libarchivehandler_la_SOURCES = $(handler)/archivehandler.cpp
    libarchivehandler_la_LIBADD = libdefaulthandler.la libcompare.la libpath.la

    libhandlerfactory_la_SOURCES = $(handler)/handlerfactory.cpp
    libhandlerfactory_la_LIBADD =  libdefaulthandler.la libarchivehandler.la libcompare.la libpath.la



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
reader_LTLIBRARIES =  libloadreader.la libwindowreader.la libzoomreader.la

    libloadreader_la_SOURCES = $(reader)/loadreader.cpp

    libzoomreader_la_SOURCES = $(reader)/zoomreader.cpp

    libwindowreader_la_SOURCES = $(reader)/windowreader.cpp
    libwindowreader_la_LIBADD = libloadreader.la libhandlerfactory.la libscrolledwindow.la libbitmap.la libbmp.la libzoomreader.la

# make libraries for gui
gui = src/gui
guidir = @abs_builddir@/gui
gui_LTLIBRARIES = libframe.la libpanel.la
    # make source and link for frame
    libframe_la_SOURCES = $(gui)/frame.cpp
    libframe_la_LIBADD = libpanel.la
    # make source and link for panel
    libpanel_la_SOURCES = $(gui)/panel.cpp
    libpanel_la_LIBADD = libwindowreader.la

window = src/window
windowdir = @abs_builddir@/window
window_LTLIBRARIES = libscrolledwindow.la
    libscrolledwindow_la_SOURCES = $(window)/scrolledwindow.cpp

bin_PROGRAMS = fmr
fmr_SOURCES = src/main/app.cpp
fmr_LDADD = libframe.la

    
