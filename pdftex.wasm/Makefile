CC=emcc
CXX=em++
DEBUGFLAGS = -O3 -s USE_ZLIB=1 -s USE_LIBPNG=1 -Wno-parentheses-equality -Wno-pointer-sign  -DWEBASSEMBLY_BUILD -fno-rtti -fno-exceptions

CFLAGS = $(DEBUGFLAGS) 
LDFLAGS =  $(DEBUGFLAGS) 
CXX_LINK = $(CXX) -o $@ $(LDFLAGS)

texsources = tex/pdftex0.c tex/pdftexini.c tex/pdftex-pool.c main.c md5.c xmemory.c texfile.c kpseemu.c bibtex.c

pdfsources = pdftexdir/avl.c    pdftexdir/utils.c      pdftexdir/writejbig2.c  pdftexdir/writettf.c \
pdftexdir/avlstuff.c  pdftexdir/pkin.c         pdftexdir/vfpacket.c   pdftexdir/writejpg.c    pdftexdir/writezip.c \
pdftexdir/epdf.c      pdftexdir/subfont.c      pdftexdir/writeenc.c   pdftexdir/writepng.c \
pdftexdir/tounicode.c    pdftexdir/writefont.c  pdftexdir/writet1.c \
pdftexdir/mapfile.c      pdftexdir/writeimg.c   pdftexdir/writet3.c

epdfsources = pdftexdir/pdftoepdf.cc

texobjects = $(texsources:.c=.o)

pdfobjects = $(pdfsources:.c=.o)

epdfobjects = $(epdfsources:.cc=.o)

swiftlatexpdftex.js: $(headers) $(texobjects) $(pdfobjects) $(epdfobjects)
	$(CXX_LINK) $(texobjects) $(pdfobjects) $(epdfobjects) xpdf/xpdf.a  --js-library library.js  --pre-js pre.js \
	 -s EXPORTED_FUNCTIONS='["_compileBibtex", "_compileLaTeX", "_compileFormat", "_main", "_setMainEntry"]' -s NO_EXIT_RUNTIME=1 -s ALLOW_MEMORY_GROWTH=1

$(texobjects): %.o: %.c
	$(CC) -c $(CFLAGS) -I. -I tex/ $< -o $@

$(pdfobjects): %.o: %.c
	$(CC) -c $(CFLAGS) -I. -I tex/ -I pdftexdir/ -Ixpdf/xpdf/ -Ixpdf/ $< -o $@

$(epdfobjects): %.o: %.cc
	$(CXX) -c $(CFLAGS) -I. -I tex/ -I pdftexdir/ -Ixpdf/xpdf/ -Ixpdf/goo/ -Ixpdf/ $< -o $@

clean:
	rm -f depend *.o pdftexdir/*.o tex/*.o swiftlatexpdftex.wasm swiftlatexpdftex.js
