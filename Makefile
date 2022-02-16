SUBDIRS := pdftex.wasm xetex.wasm

all: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: all $(SUBDIRS)
