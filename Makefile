ifeq ($(HOST),)
CXX 		:= g++
wxCONFIG	:= wx-config
endif
ifeq ($(HOST),windows)
CXX			:= x86_64-w64-mingw32-g++
wxCONFIG 	:= /usr/x86_64-w64-mingw32/bin/wx-config
endif

CXXFLAGS 	:= --std=c++17 -Wall

wxFLAGS		:= `$(wxCONFIG) --cxxflags base,core`
wxLIBS		:= `$(wxCONFIG) --libs base,core`

ifeq ($(HOST),)
exe 		:= fmr
soext		:= so
endif
ifeq ($(HOST),windows)
exe			:= fmr.exe
soext		:= dll
endif

RELEASE 	:= DEBUG
ifeq ($(RELEASE), DEBUG)
CXXFLAGS 	+= -g
endif


gui 		:= panel frame
handler 	:= archivehandler defaulthandler handlerfactory 
bitmap		:= bitmap
reader 		:= windowreader threadreader 
src			:= $(addprefix handler/, $(handler) ) $(addprefix bitmap/, $(bitmap) ) $(addprefix image/, $(image) ) $(addprefix reader/, $(reader) ) $(addprefix gui/, $(gui) )
# src			:= handler/handler handler/filehandler handler/archivehandler reader/image reader/reader gui/panel gui/frame
header		:= $(addsuffix .h, $(src) )
VPATH		:= src:build
so			:= $(addsuffix .$(soext), $(src) )
obj			:= $(addsuffix .o, $(src) )

.PHONY: clean all testing/scroll.cpp

all: build $(obj) $(so) $(exe) 

build : 
	mkdir $(addprefix build/, handler reader handler image gui bitmap ) -p

$(exe) : FLAGS := -Isrc $(CXXFLAGS) `$(wxCONFIG) --libs --cxxflags base,core`


$(exe) : LIBS := $(src)
# building executable file
$(exe) 		: main/app.cpp main/app.h gui/frame.h base/config.h
	$(CXX) $< $(INCLUDE) $(addprefix build/, $(addsuffix .$(soext), $(LIBS) ) ) $(FLAGS)  -o $(addprefix build/, $@ )


$(so) 		: FLAGS := -Isrc $(CXXFLAGS) $(wxLIBS)
$(obj)		: FLAGS	:= -Isrc -c $(CXXFLAGS) $(wxFLAGS)

LINK := SHARED
ifeq ($(LINK), SHARED)
$(obj)		: FLAGS += -fPIC
$(so)		: FLAGS += -shared
endif

$(src) : % : %.o %.$(soext)

link : $(so)
gui/frame.$(soext) : lib += gui/panel
gui/panel.$(soext) : lib += reader/windowreader
bitmap/bitmapvertical.$(soext) : lib += bitmap/bitmap
reader/windowreader.$(soext) : lib += bitmap/bitmap
reader/threadreader.$(soext) : lib += handler/handlerfactory bitmap/bitmap
handler/handler.o	: lib += handler/defaulthandler handler/archivehandler

$(filter-out config.$(soext), $(so)) : libs += base/config

# building Shared Object
$(addprefix build/, $(so) ) : build/%.$(soext): %.o
	$(CXX) $< $(addprefix build/, $(addsuffix .so, $(lib) ) ) $(CXXFLAGS) -g -shared $(FLAGS) -o $@

# $(obj) : FLAGS := $(CXXFLAGS) $(wxFLAGS)

(filter-out $(addprefix bitmap/, bmp.o bitmap.o), $(bitmap) : $(addprefix bitmap, bmp.h bitmap.h)
handler/handler.o 	: handler/defaulthandler.h handler/archivehandler.h
reader/threadreader.o	: $(addprefix handler/, $(addsuffix .h, $(handler) ) ) bitmap/bitmap.h
reader/windowreader.o 	: bitmap/bitmap.h 
gui/panel.o : reader/windowreader.h 
gui/frame.o : gui/panel.h
main/app.o : gui/frame.h

$(filter-out config.o, $(obj) ) : base/config.h
$(addsuffix .o, $(addprefix handler/, $(handler) ) ) : base/path.h base/vector.h
obj : $(obj)

$(addprefix build/, $(obj) ): build/%.o : %.cpp %.h 
	$(CXX) $< $(FLAGS) -o $@

asd:
	# $(CXX) $< $(cpp) -c $(CXXFLAGS) -fPIC -g -o $@

clean: cleanobj cleanso
	rm -f $(addprefix build/, $(exe))

cleanobj :
	rm -f $(addprefix build/, $(obj) )

cleanso :
	rm -f $(addprefix build/, $(so) )
