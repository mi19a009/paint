# Make
PAINT  =$(TARGET)/paint
SHARE  =$(TARGET)/libshare.a
TARGET =build
VIEWER =$(TARGET)/viewer
export CFLAGS TARGET
.PHONY: all clean debug msg paint release share viewer
all: msg paint viewer
paint: $(PAINT)
share: $(SHARE)
viewer: $(VIEWER)
clean:
	@rm -rf build debug locale release
	@cd msg && $(MAKE) $@
	@cd paint && $(MAKE) $@
	@cd share && $(MAKE) $@
	@cd viewer && $(MAKE) $@
debug:
	@$(MAKE) TARGET=$@ "CFLAGS=$(CFLAGS) -g -DG_ENABLE_DEBUG"
release:
	@$(MAKE) TARGET=$@ "CFLAGS=$(CFLAGS) -O2 -DNDEBUG -DG_DISABLE_ASSERT -DG_DISABLE_CAST_CHECKS"
msg:
	@cd msg && $(MAKE)
$(PAINT): $(SHARE)
	@cd paint && $(MAKE)
$(SHARE):
	@cd share && $(MAKE)
$(VIEWER): $(SHARE)
	@cd viewer && $(MAKE)
