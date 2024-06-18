PROJECT_NAME 	:= 	SwiftLaTeX

SUBDIRS			:=	pdftex.wasm \
					xetex.wasm \
					dvipdfm.wasm

_default:
	@echo -e "\033[33m[BUILDING]\033[0m $(PROJECT_NAME)"
	@$(MAKE) all --no-print-directory

all: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@ -j --no-print-directory && \
	echo -e "\033[32m[OK]\033[0m $@" || \
	echo -e "\033[31m[ERROR]\033[0m $@"

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

fclean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir fclean; \
	done

re: fclean all

.PHONY: all clean fclean re $(SUBDIRS)
.SILENT: all clean fclean re $(SUBDIRS)
