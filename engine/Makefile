CC=emcc
CXX=em++
DEBUGFLAGS = -O3
CFLAGS = $(DEBUGFLAGS) -Iprebuilt/graphite/include -s USE_FREETYPE=1 -s USE_ICU=1 -I prebuilt/graphite2/include -I prebuilt/harfbuzz/include
LDFLAGS =  $(DEBUGFLAGS)  --js-library library.js -s USE_FREETYPE=1 -s USE_ICU=1 \
 prebuilt/graphite2/libgraphite2.a prebuilt/harfbuzz/libharfbuzz.a  --pre-js pre.js \
 -s EXPORTED_FUNCTIONS='["_comileBibtex", "_compileLaTeX", "_compileFormat", "_main"]' -s NO_EXIT_RUNTIME=1 \
  -s WASM=1 -s ALLOW_MEMORY_GROWTH=1
CXX_LINK = $(CXX) -o $@ $(LDFLAGS) 


csources = xetex-io.c xetex-xetex0.c xetex-ini.c xetex-texmfmp.c xetex-errors.c \
xetex-scaledmath.c xetex-output.c xetex-stringpool.c core-memory.c core-bridge.c \
xetex-pagebuilder.c xetex-shipout.c xetex-math.c xetex-ext.c xetex-linebreak.c \
 xetex-pic.c bibtex.c md5.c xetex-wasm.c miniz.c
 
cppsources = teckit-Engine.cpp xetex-XeTeXOTMath.cpp xetex-XeTeXLayoutInterface.cpp \
 xetex-XeTeXFontMgr.cpp xetex-XeTeXFontInst.cpp xetex-XeTeXFontMgr_FC.cpp icustubdata.cpp

headers = xetex-XeTeXFontMgr.h xetex-XeTeXLayoutInterface.h xetex-XeTeXOTMath.h xetex-XeTeXFontMgr_FC.h \
			xetex-XeTeXFontInst.h xetex-xetexd.h xetex-web.h xetex-swap.h xetex-stringpool.h xetex-ext.h \
			xetex-core.h xetex-constants.h teckit-Format.h teckit-cxx-Engine.h teckit-Compiler.h teckit-Common.h \
			teckit-c-Engine.h md5.h core-memory.h core-strutils.h core-foundation.h core-bridge.h bibtex.h

cobjects = $(csources:.c=.o)

cppobjects = $(cppsources:.cpp=.o)

swiftlatex.js: $(headers) $(cobjects) $(cppobjects) $(graphiteobjects)
	$(CXX_LINK) $(cobjects) $(cppobjects) $(graphiteobjects) 

$(cobjects): %.o: %.c
	$(CC) -c $(CFLAGS) -I. $< -o $@

$(cppobjects): %.o: %.cpp
	$(CXX) -std=c++11 -c $(CFLAGS) -I.  $< -o $@

clean:
	rm -f *.o  swiftlatex*
	
