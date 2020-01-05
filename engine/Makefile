CC=clang
CXX=clang++
DEBUGFLAGS = -g -O3
CFLAGS = $(DEBUGFLAGS) -DNATIVE_BUILD -I/usr/include/freetype2 -I/usr/include/harfbuzz -I.
LDFLAGS =  $(DEBUGFLAGS)  
CXX_LINK = $(CXX) -o $@ $(LDFLAGS) 

csources = xetex-io.c xetex-xetex0.c xetex-ini.c xetex-texmfmp.c xetex-errors.c \
xetex-scaledmath.c xetex-output.c xetex-stringpool.c core-memory.c core-bridge.c \
xetex-pagebuilder.c xetex-shipout.c xetex-math.c xetex-ext.c xetex-linebreak.c \
 xetex-pic.c bibtex.c md5.c xetex-wasm.c miniz.c
 
cppsources = teckit-Engine.cpp xetex-XeTeXOTMath.cpp xetex-XeTeXLayoutInterface.cpp \
 xetex-XeTeXFontMgr.cpp xetex-XeTeXFontInst.cpp xetex-XeTeXFontMgr_FC.cpp


headers = xetex-XeTeXFontMgr.h xetex-XeTeXLayoutInterface.h xetex-XeTeXOTMath.h xetex-XeTeXFontMgr_FC.h \
			xetex-XeTeXFontInst.h xetex-xetexd.h xetex-web.h xetex-swap.h xetex-stringpool.h xetex-ext.h \
			xetex-core.h xetex-constants.h teckit-Format.h teckit-cxx-Engine.h teckit-Compiler.h teckit-Common.h \
			teckit-c-Engine.h md5.h core-memory.h core-strutils.h core-foundation.h core-bridge.h bibtex.h

cobjects = $(csources:.c=.o)

cppobjects = $(cppsources:.cpp=.o)


swiftlatex: $(headers) $(cobjects) $(cppobjects) 
	$(CXX_LINK) $(cobjects) $(cppobjects) -lgraphite2 -lfreetype -lharfbuzz -licuuc   -lharfbuzz-icu  -lcurl

$(cobjects): %.o: %.c
	$(CC) -c $(CFLAGS)  $< -o $@

$(cppobjects): %.o: %.cpp
	$(CXX) -c $(CFLAGS)   $< -o $@



clean:
	rm -f *.o swiftlatex
	
