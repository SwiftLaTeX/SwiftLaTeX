PROJECT_NAME	:=	swiftlatexpdftex.js

CC          	= 	emcc
CXX          	= 	em++
CFLAGS       	= 	-O3 \
					-s USE_ZLIB=1 -s USE_LIBPNG=1 \
					-Wno-parentheses-equality -Wno-pointer-sign -fno-rtti -fno-exceptions \
					-DWEBASSEMBLY_BUILD

CXX_LINK     	= 	$(CXX) -o $@ $(CFLAGS)

TEXSOURCES   	= 	tex/pdftex0.c \
					tex/pdftexini.c \
					tex/pdftex-pool.c \
					main.c \
					md5.c \
					xmemory.c \
					texfile.c \
					kpseemu.c \
					bibtex.c

PDFSOURCES   	= 	pdftexdir/avl.c \
					pdftexdir/utils.c \
					pdftexdir/writejbig2.c \
					pdftexdir/writettf.c \
					pdftexdir/avlstuff.c \
					pdftexdir/pkin.c \
					pdftexdir/vfpacket.c \
					pdftexdir/writejpg.c \
					pdftexdir/writezip.c \
					pdftexdir/epdf.c \
					pdftexdir/subfont.c \
					pdftexdir/writeenc.c \
					pdftexdir/writepng.c \
					pdftexdir/tounicode.c \
					pdftexdir/writefont.c \
					pdftexdir/writet1.c \
					pdftexdir/mapfile.c \
					pdftexdir/writeimg.c \
					pdftexdir/writet3.c

EPDFSOURCES  	= 	pdftexdir/pdftoepdf.cc

BUILD_DIR    	=	build

TEXOBJECTS   	= 	$(TEXSOURCES:%.c=$(BUILD_DIR)/%.o)

PDFOBJECTS   	= 	$(PDFSOURCES:%.c=$(BUILD_DIR)/%.o)

EPDFOBJECTS  	= 	$(EPDFSOURCES:%.cc=$(BUILD_DIR)/%.o)

_default:
	@$(MAKE) all --no-print-directory -j

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(HEADERS) $(TEXOBJECTS) $(PDFOBJECTS) $(EPDFOBJECTS)
	@$(CXX_LINK) $(TEXOBJECTS) $(PDFOBJECTS) $(EPDFOBJECTS) xpdf/xpdf.a  --js-library library.js  --pre-js pre.js \
	-s EXPORTED_FUNCTIONS='["_compileBibtex", "_compileLaTeX", "_compileFormat", "_main", "_setMainEntry", "_malloc"]' \
	-s EXPORTED_RUNTIME_METHODS=["cwrap"] -s NO_EXIT_RUNTIME=1 -s ALLOW_MEMORY_GROWTH=1 && \
	echo -e "\033[32m[DONE]\033[0m $(PROJECT_NAME)" || \
	echo -e "\033[31m[ERROR]\033[0m $(PROJECT_NAME)"

$(TEXOBJECTS): $(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) -I. -I tex/ $< -o $@ && \
	echo -e "\033[32m[OK]\033[0m $@" || \
	echo -e "\033[31m[ERROR]\033[0m $@"

$(PDFOBJECTS): $(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) -I. -I tex/ -I pdftexdir/ -Ixpdf/xpdf/ -Ixpdf/ $< -o $@ && \
	echo -e "\033[32m[OK]\033[0m $@" || \
	echo -e "\033[31m[ERROR]\033[0m $@"

$(EPDFOBJECTS): $(BUILD_DIR)/%.o : %.cc
	@mkdir -p $(dir $@)
	@$(CXX) -c $(CFLAGS) -I. -I tex/ -I pdftexdir/ -Ixpdf/xpdf/ -Ixpdf/goo/ -Ixpdf/ $< -o $@ && \
	echo -e "\033[32m[OK]\033[0m $@" || \
	echo -e "\033[31m[ERROR]\033[0m $@"

clean:
	@rm -rf $(BUILD_DIR)
	@echo -e "\033[32m[CLEANED]\033[0m $(PROJECT_NAME)"

fclean: clean
	@rm -f $(PROJECT_NAME)
	@echo -e "\033[32m[FCLEAN]\033[0m $(PROJECT_NAME)"

re: fclean _default

.PHONY: all clean fclean re
.SILENT: all clean fclean re
