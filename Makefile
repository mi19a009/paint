# Make
TARGET =build
export CFLAGS TARGET
.PHONY: all clean debug msg paint release share viewer
all: msg paint viewer
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
paint: share
	@cd paint && $(MAKE)
share:
	@cd share && $(MAKE)
viewer: share
	@cd viewer && $(MAKE)
