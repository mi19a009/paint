# Make
TARGET =build
export CFLAGS TARGET
.PHONY: all clean debug draw install msg paint release share uninstall viewer
all: msg draw paint viewer
clean:
	@rm -rf build debug locale release
	@cd draw && $(MAKE) $@
	@cd msg && $(MAKE) $@
	@cd paint && $(MAKE) $@
	@cd share && $(MAKE) $@
	@cd viewer && $(MAKE) $@
uninstall:
	@cd draw && $(MAKE) $@
	@cd paint && $(MAKE) $@
	@cd viewer && $(MAKE) $@
debug:
	@$(MAKE) TARGET=$@ "CFLAGS=$(CFLAGS) -g -DG_ENABLE_DEBUG"
release:
	@$(MAKE) TARGET=$@ "CFLAGS=$(CFLAGS) -O2 -DNDEBUG -DG_DISABLE_ASSERT -DG_DISABLE_CAST_CHECKS"
msg share:
	@cd $@ && $(MAKE)
draw paint viewer: share
	@cd $@ && $(MAKE)
