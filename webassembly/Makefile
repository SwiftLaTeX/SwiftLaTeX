CC=emcc
CXX=em++
DEBUGFLAGS = -O3
CFLAGS = $(DEBUGFLAGS) -Wno-parentheses-equality -Wno-pointer-sign -DWEBASSEMBLY_BUILD -s \
 USE_FREETYPE=1 -s USE_HARFBUZZ=1 -s USE_ICU=1 -s USE_LIBPNG=1 -fno-rtti -fno-exceptions
LDFLAGS =  $(DEBUGFLAGS)  --js-library library.js -s USE_FREETYPE=1 \
 -s USE_HARFBUZZ=1 -s USE_LIBPNG=1 --pre-js pre.js \
 -s EXPORTED_FUNCTIONS='["_compileBibtex", "_compileLaTeX", "_compileFormat", "_main", "_setMainEntry"]' -s NO_EXIT_RUNTIME=1 \
  -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -fno-rtti -fno-exceptions
CXX_LINK = $(CXX) -o $@ $(LDFLAGS) 

texsources = tex/xetex0.c tex/xetexini.c tex/xetex-pool.c  \
md5.c xmemory.c texfile.c kpseemu.c texmfmp.c main.c \
bibtex.c xetexdir/XeTeX_ext.c xetexdir/XeTeX_pic.c xetexdir/image/bmpimage.c xetexdir/image/jpegimage.c \
xetexdir/image/pngimage.c xetexdir/trans.c
			
xetexsources = xetexdir/XeTeXOTMath.cpp \
xetexdir/XeTeXLayoutInterface.cpp \
xetexdir/XeTeXFontMgr.cpp xetexdir/XeTeXFontInst.cpp \
xetexdir/XeTeXFontMgr_FC.cpp xetexdir/hz.cpp \
xetexdir/XeTeXHTML.cpp xetexdir/XeTeXColor.cpp \
xetexdir/pdfimage.cpp

teckitsources = teckit/teckit-Engine.cpp 

texobjects = $(texsources:.c=.o)

xetexobjects = $(xetexsources:.cpp=.o)

teckitobjects = $(teckitsources:.cpp=.o)

swiftlatex.js: $(texobjects) $(xetexobjects) $(teckitobjects) 
	$(CXX_LINK) $(texobjects) $(xetexobjects) $(teckitobjects) xpdf/xpdf.a

$(texobjects): %.o: %.c
	$(CC) -c $(CFLAGS) -I. -I tex/ $< -o $@

$(xetexobjects): %.o: %.cpp
	$(CXX) -c $(CFLAGS) -I. -I tex/ -I xetexdir/ -Ixpdf/ -Ixpdf/goo/ -Ixpdf/xpdf/ $< -o $@

$(teckitobjects): %.o: %.cpp
	$(CXX) -c $(CFLAGS) -I. -I teckit/ $< -o $@

clean:
	rm -f *.o tex/*.o xetexdir/*.o xetexdir/image/*.o teckit/*.o swiftlatex
	
