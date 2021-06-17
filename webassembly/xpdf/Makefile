CC=emcc
CXX=em++
DEBUGFLAGS = -O3
CFLAGS = $(DEBUGFLAGS)  -fno-rtti -fno-exceptions

xpdfsources = \
goo/FixedPoint.cc  goo/GHash.cc  goo/gmem.cc    goo/GString.cc \
goo/gfile.cc       goo/GList.cc  goo/gmempp.cc \
xpdf/Array.cc \
xpdf/Lexer.cc  \
xpdf/Catalog.cc                  \
xpdf/Stream.cc \
xpdf/Object.cc  \
xpdf/TextString.cc \
xpdf/Dict.cc                           \
xpdf/Error.cc               xpdf/Page.cc               \
xpdf/Parser.cc                                   \
xpdf/PDFDoc.cc             \
xpdf/UTF8.cc \
xpdf/XRef.cc \

xpdfobjects = $(xpdfsources:.cc=.o)


xpdflib: $(xpdfobjects) 
	emar rcs xpdf.a $(xpdfobjects) 

$(xpdfobjects): %.o: %.cc
	$(CXX) -c $(CFLAGS) -I. -Ifofi/ -Igoo/ -Ixpdf/ -Isplash/ $< -o $@

clean:
	rm -f *.o fofi/*.o goo/*.o xpdf/*.o *.a xpdflib
	
