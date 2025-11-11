# Make
ICONS  =$(wildcard ../icons/48x48/actions/*.png)
PROJ   =paint viewer
RES    =$(addsuffix /resource.c, $(PROJ))
TARGET =build
export CFLAGS TARGET
.PHONY: all clean debug msg release
all:
	@cd paint  && $(MAKE)
	@cd viewer && $(MAKE)
clean:
	@rm -rf build debug locale msg release
	@cd paint  && $(MAKE) $@
	@cd viewer && $(MAKE) $@
debug:
	@$(MAKE) TARGET=$@ "CFLAGS=$(CFLAGS) -g -DG_ENABLE_DEBUG"
release:
	@$(MAKE) TARGET=$@ "CFLAGS=$(CFLAGS) -O2 -DNDEBUG -DG_DISABLE_ASSERT -DG_DISABLE_CAST_CHECKS"
msg: msg.py msg.csv
	@mkdir -p $@
	@python3 $<
$(RES): %/resource.c: %.gresource.xml $(wildcard %/*.ui) $(ICONS)
	@echo $@
	@glib-compile-resources --generate-source --target $@ $<
