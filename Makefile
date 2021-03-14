#############################################
##                                         ##
##    Copyright (C) 2019-2019 Julian Uy    ##
##  https://sites.google.com/site/awertyb  ##
##                                         ##
##   See details of license at "LICENSE"   ##
##                                         ##
#############################################

CC = i686-w64-mingw32-gcc
CXX = i686-w64-mingw32-g++
AR = i686-w64-mingw32-ar
WINDRES = i686-w64-mingw32-windres
GIT_TAG := $(shell git describe --abbrev=0 --tags)
INCFLAGS += -I. -I.. -Iexternal/dav1d/include -Iexternal/dav1d/build/include/dav1d -Iexternal/libavif/include
ALLSRCFLAGS += $(INCFLAGS) -DGIT_TAG=\"$(GIT_TAG)\"
CFLAGS += -O3 -flto
CFLAGS += $(ALLSRCFLAGS) -Wall -Wno-unused-value -Wno-format -DNDEBUG -DWIN32 -D_WIN32 -D_WINDOWS 
CFLAGS += -D_USRDLL -DUNICODE -D_UNICODE 
CFLAGS += -DAVIF_CODEC_DAV1D=1
CXXFLAGS += $(CFLAGS) -fpermissive
WINDRESFLAGS += $(ALLSRCFLAGS) --codepage=65001
LDFLAGS += -static -static-libgcc -shared -Wl,--kill-at
LDLIBS +=

%.o: %.c
	@printf '\t%s %s\n' CC $<
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.cpp
	@printf '\t%s %s\n' CXX $<
	$(CXX) -c $(CXXFLAGS) -o $@ $<

%.o: %.rc
	@printf '\t%s %s\n' WINDRES $<
	$(WINDRES) $(WINDRESFLAGS) $< $@

LIBDAV1D_SOURCES += external/dav1d/build/src/libdav1d.a
LIBAVIF_SOURCES += external/libavif/src/codec_dav1d.c external/libavif/src/alpha.c external/libavif/src/avif.c external/libavif/src/colr.c external/libavif/src/io.c external/libavif/src/mem.c external/libavif/src/obu.c external/libavif/src/rawdata.c external/libavif/src/read.c external/libavif/src/reformat.c external/libavif/src/reformat_libyuv.c external/libavif/src/stream.c external/libavif/src/utils.c external/libavif/src/write.c
SOURCES := extractor.c spi00in.c ifavif.rc $(LIBAVIF_SOURCES) $(LIBDAV1D_SOURCES)
OBJECTS := $(SOURCES:.c=.o)
OBJECTS := $(OBJECTS:.cpp=.o)
OBJECTS := $(OBJECTS:.rc=.o)

BINARY ?= ifavif.spi
ARCHIVE ?= ifavif.$(GIT_TAG).7z

all: $(BINARY)

archive: $(ARCHIVE)

clean:
	rm -f $(OBJECTS) $(BINARY) $(ARCHIVE)
	rm -rf external/dav1d/build

external/dav1d/build/include/dav1d/version.h: external/dav1d/build/src/libdav1d.a

external/libavif/src/codec_dav1d.c: external/dav1d/build/include/dav1d/version.h

external/dav1d/build/src/libdav1d.a:
	cd external/dav1d && meson build --buildtype release -Ddefault_library=static --cross-file ../meson_toolchains/mingw32_meson.ini && ninja -C build

$(ARCHIVE): $(BINARY) 
	rm -f $(ARCHIVE)
	7z a $@ $^

$(BINARY): $(OBJECTS) 
	@printf '\t%s %s\n' LNK $@
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
