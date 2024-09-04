PROJECT_NAME 	:= 	swiftlatexxetex.js
CC 				= 	emcc
CXX 			= 	em++
DEBUGFLAGS 		= 	-O3

CFLAGS 			= 	$(DEBUGFLAGS) -Wno-parentheses-equality -Wno-pointer-sign -DWEBASSEMBLY_BUILD \
					-s USE_FREETYPE=1 -s USE_ICU=1 -s USE_LIBPNG=1 -fno-rtti -fno-exceptions

LDFLAGS 		= 	$(DEBUGFLAGS) --js-library library.js \
					-s USE_FREETYPE=1 \
					-s USE_ICU=1 \
					-s USE_LIBPNG=1 \
					--pre-js pre.js \
					-s ENVIRONMENT="web" \
					-s EXPORTED_FUNCTIONS='["_compileBibtex", "_compileLaTeX", "_compileFormat", "_main", "_setMainEntry", "_malloc"]' -s NO_EXIT_RUNTIME=1 \
					-s WASM=1 -s EXPORTED_RUNTIME_METHODS=["cwrap"] -s ALLOW_MEMORY_GROWTH=1

LINKFLAG 		= 	$(CXX) -o $@ $(LDFLAGS)

BUILD_DIR 		= 	build

TEXSOURCES 		= 	tex/xetex0.c tex/xetexini.c tex/xetex-pool.c  \
					md5.c xmemory.c texfile.c kpseemu.c texmfmp.c main.c \
					bibtex.c xetexdir/XeTeX_ext.c xetexdir/XeTeX_pic.c xetexdir/image/bmpimage.c xetexdir/image/jpegimage.c \
					xetexdir/image/pngimage.c xetexdir/trans.c

XETEXSOURCES 	= 	xetexdir/XeTeXOTMath.cpp \
					xetexdir/XeTeXLayoutInterface.cpp \
					xetexdir/XeTeXFontMgr.cpp xetexdir/XeTeXFontInst.cpp \
					xetexdir/XeTeXFontMgr_FC.cpp xetexdir/hz.cpp \
					xetexdir/pdfimage.cpp

TECKITSOURCES 	= 	teckit/teckit-Engine.cpp

TEXOBJECTS 		= 	$(TEXSOURCES:%.c=$(BUILD_DIR)/%.o)
XETEXOBJECTS 	= 	$(XETEXSOURCES:%.cpp=$(BUILD_DIR)/%.o)
TECKITOBJECTS 	= 	$(TECKITSOURCES:%.cpp=$(BUILD_DIR)/%.o)

_default:
	@$(MAKE) all --no-print-directory -j

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(TEXOBJECTS) $(XETEXOBJECTS) $(TECKITOBJECTS)
	@$(LINKFLAG) $(TEXOBJECTS) $(XETEXOBJECTS) $(TECKITOBJECTS) xpdf/xpdf.a graphite2/libgraphite2.a harfbuzz/libharfbuzz.a && \
	echo -e "\033[32m[DONE]\033[0m $(PROJECT_NAME)" || \
	echo -e "\033[31m[ERROR]\033[0m $(PROJECT_NAME)"

$(TEXOBJECTS): $(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	@$(CC) -c $(CFLAGS) -I. -I tex/ -I harfbuzz/ $< -o $@ && \
	echo -e "\033[32m[OK]\033[0m $<" || \
	echo -e "\033[31m[ERROR]\033[0m $<"

$(XETEXOBJECTS): $(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@$(CXX) -c $(CFLAGS) -I. -I tex/ -I xetexdir/ -I harfbuzz/  -I xpdf/ -I xpdf/goo/ -I xpdf/xpdf/ $< -o $@ && \
	echo -e "\033[32m[OK]\033[0m $<" || \
	echo -e "\033[31m[ERROR]\033[0m $<"

$(TECKITOBJECTS): $(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@$(CXX) -c $(CFLAGS) -I. -I teckit/ $< -o $@ && \
	echo -e "\033[32m[OK]\033[0m $<" || \
	echo -e "\033[31m[ERROR]\033[0m $<"

clean:
	@rm -rf $(BUILD_DIR) && \
	echo -e "\033[32m[CLEANED]\033[0m $(BUILD_DIR)" || \
	echo -e "\033[31m[ERROR]\033[0m $(BUILD_DIR)"

fclean: clean
	@rm -f $(PROJECT_NAME) && \
	echo -e "\033[32m[CLEANED]\033[0m $(PROJECT_NAME)" || \
	echo -e "\033[31m[ERROR]\033[0m $(PROJECT_NAME)"

re: fclean _default

.PHONY: all clean fclean re
.SILENT:
