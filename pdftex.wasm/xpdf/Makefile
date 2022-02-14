CC=emcc
CXX=em++
DEBUGFLAGS = -O3
CFLAGS = $(DEBUGFLAGS) 

xpdfsources = \
goo/FixedPoint.cc  goo/GHash.cc  goo/gmem.cc    goo/GString.cc \
goo/gfile.cc       goo/GList.cc  goo/gmempp.cc \
fofi/FoFiBase.cc       fofi/FoFiIdentifier.cc  fofi/FoFiType1.cc \
fofi/FoFiEncodings.cc  fofi/FoFiTrueType.cc    fofi/FoFiType1C.cc \
xpdf/Array.cc \
xpdf/Annot.cc \
xpdf/Lexer.cc  \
xpdf/Catalog.cc                  \
xpdf/Stream.cc \
xpdf/Object.cc  \
xpdf/TextString.cc \
xpdf/Dict.cc                           \
xpdf/Error.cc              \
xpdf/Page.cc               \
xpdf/Parser.cc             \
xpdf/PDFDoc.cc             \
xpdf/UTF8.cc \
xpdf/XRef.cc \
xpdf/GfxFont.cc \
xpdf/Link.cc \
xpdf/GlobalParams.cc \
xpdf/CharCodeToUnicode.cc \
xpdf/PSTokenizer.cc \
xpdf/NameToCharCode.cc \
xpdf/UnicodeMap.cc \
xpdf/UnicodeRemapping.cc \
xpdf/FontEncodingTables.cc \
xpdf/PDFDocEncoding.cc \
xpdf/BuiltinFontTables.cc \
xpdf/BuiltinFont.cc \
xpdf/CMap.cc \
xpdf/OptionalContent.cc \
xpdf/JBIG2Stream.cc \
xpdf/JPXStream.cc \
xpdf/JArithmeticDecoder.cc \
xpdf/Decrypt.cc \
xpdf/SecurityHandler.cc \
xpdf/Form.cc \
xpdf/XFAForm.cc \
xpdf/AcroForm.cc \
xpdf/Zoox.cc



xpdfobjects = $(xpdfsources:.cc=.o)


xpdflib: $(xpdfobjects) 
	emar rcs xpdf.a $(xpdfobjects) 

$(xpdfobjects): %.o: %.cc
	$(CXX) -c $(CFLAGS) -I. -Ifofi/ -Igoo/ -Ixpdf/ -Isplash/ $< -o $@

clean:
	rm -f *.o fofi/*.o goo/*.o xpdf/*.o xpdflib
	
