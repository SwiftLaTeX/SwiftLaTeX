CC=gcc
CXX=g++
DEBUGFLAGS = -g 
CFLAGS = $(DEBUGFLAGS) -I/usr/include/freetype2 -I/usr/include/harfbuzz 
LDFLAGS =  $(DEBUGFLAGS)  
CXX_LINK = $(CXX) -o $@ $(LDFLAGS) 


csources = xetex-io.c xetex-xetex0.c xetex-ini.c xetex-texmfmp.c xetex-errors.c \
xetex-scaledmath.c xetex-output.c xetex-stringpool.c core-memory.c core-bridge.c \
xetex-pagebuilder.c xetex-shipout.c xetex-math.c xetex-ext.c xetex-linebreak.c \
 xetex-pic.c bibtex.c md5.c xetex-wasm.c
 
cppsources = teckit-Engine.cpp xetex-XeTeXOTMath.cpp xetex-XeTeXLayoutInterface.cpp \
 xetex-XeTeXFontMgr.cpp xetex-XeTeXFontInst.cpp xetex-XeTeXFontMgr_FC.cpp



cobjects = $(csources:.c=.o)

cppobjects = $(cppsources:.cpp=.o)

wasmtex: $(headers) $(cobjects) $(cppobjects)
	$(CXX_LINK) $(cobjects) $(cppobjects) -lfreetype -lharfbuzz -lz   -licuuc   -lharfbuzz-icu  -lcurl


$(cobjects): %.o: %.c
	$(CC) -c $(CFLAGS) -I. $< -o $@

$(cppobjects): %.o: %.cpp
	$(CXX) -c $(CFLAGS) -I. $< -o $@



clean:
	rm -f *.o *.bc
	
