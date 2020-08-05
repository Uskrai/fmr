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

src			:= config filehandler reader/image reader/reader panel frame
header		:= $(addsuffix .h, $(src) )
VPATH		:= src:build
so			:= $(addsuffix .$(soext), $(src) )
obj			:= $(addsuffix .o, $(src) )

.PHONY: clean all testing/scroll.cpp

all: build build/reader $(obj) $(so) $(exe) 

build: 
	mkdir build build/reader


$(exe) : FLAGS := -Isrc $(CXXFLAGS) `$(wxCONFIG) --libs --cxxflags base,core`


$(exe) : LIBS := $(src)
# building executable file
$(exe) 		: app.cpp frame.h
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
frame.$(soext) : lib += panel
panel.$(soext) : lib += reader/reader
reader/reader.$(soext) : lib += reader/image
reader/image.$(soext) : lib += filehandler

$(filter-out config.$(soext), $(so)) : libs += config

# building Shared Object
$(addprefix build/, $(so) ) : build/%.$(soext): %.o
	$(CXX) $< $(addprefix build/, $(addsuffix .so, $(lib) ) ) $(CXXFLAGS) -g -shared $(FLAGS) -o $@

# $(obj) : FLAGS := $(CXXFLAGS) $(wxFLAGS)

reader/image,o		: filehandler.h
reader/reader.o 	: reader/image.h
panel.o : reader/reader.h 
frame.o : panel.h
app.o : frame.h

$(filter-out config.o, $(obj) ) : config.h
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
