SUBDIRS := pdftex.wasm xetex.wasm dvipdfm.wasm

all: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: all $(SUBDIRS)
