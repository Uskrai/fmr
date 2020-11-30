
VPATH = @srcdir@/src
AM_CPPFLAGS = -I@srcdir@/src

if SHARED
    # sds
endif

base = base
basedir = @abs_builddir@/base
base_LTLIBRARIES = libcompare.la libpath.la

    libcompare_la_SOURCES = $(base)/compare.cpp

    libpath_la_SOURCES = $(base)/path.cpp


handler = handler
handlerdir = @abs_builddir@/handler
handler_LTLIBRARIES = libdefaulthandler.la libarchivehandler.la libhandlerfactory.la

    libdefaulthandler_la_SOURCES = $(handler)/defaulthandler.cpp

    libarchivehandler_la_SOURCES = $(handler)/archivehandler.cpp
    libarchivehandler_la_LIBADD = libdefaulthandler.la libcompare.la libpath.la

    libhandlerfactory_la_SOURCES = $(handler)/handlerfactory.cpp
    libhandlerfactory_la_LIBADD =  libdefaulthandler.la libarchivehandler.la libcompare.la libpath.la



# make libraries for bitmap
bitmap = bitmap
bitmapdir = @abs_builddir@/bitmap
bitmap_LTLIBRARIES = libbitmap.la libsize.la libposition.la libbmp.la

    libbmp_la_SOURCES = $(bitmap)/bmp.cpp
    # make source and link for bitmap
    libbitmap_la_SOURCES = $(bitmap)/bitmap.cpp
    libbitmap_la_LIBADD = libsize.la libposition.la libbmp.la

    libsize_la_SOURCES = $(bitmap)/size.cpp libbmp.la

    libposition_la_SOURCES = $(bitmap)/position.cpp libbmp.la

reader = reader
readerdir = @abs_builddir@/reader   
reader_LTLIBRARIES =  libthreadreader.la libwindowreader.la

    libthreadreader_la_SOURCES = $(reader)/threadreader.cpp

    libwindowreader_la_SOURCES = $(reader)/windowreader.cpp
    libwindowreader_la_LIBADD = libthreadreader.la libhandlerfactory.la libbitmap.la libbmp.la

# make libraries for gui
gui = gui
guidir = @abs_builddir@/gui
gui_LTLIBRARIES = libframe.la libpanel.la
    # make source and link for frame
    libframe_la_SOURCES = $(gui)/frame.cpp
    libframe_la_LIBADD = libpanel.la
    # make source and link for panel
    libpanel_la_SOURCES = $(gui)/panel.cpp
    libpanel_la_LIBADD = libwindowreader.la


bin_PROGRAMS = fmr
fmr_SOURCES = main/app.cpp
fmr_LDADD = libframe.la
# fmr_LDADD  = libframe.la libpanel.la libwindowreader.la libbitmap.la libsize.la libposition.la  libhandlerfactory.la libdefaulthandler.la libarchivehandler.la

    
