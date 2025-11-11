# Make
ICONS   =$(wildcard ../icons/48x48/actions/*.png)
LINGUAS =ja
MSG     =$(addprefix msg/, $(addsuffix .po, $(LINGUAS)))
MSGOBJ  =$(addprefix locale/, $(addsuffix /LC_MESSAGES/msg.mo, $(LINGUAS)))
PROJ    =paint viewer
RES     =$(addsuffix /resource.c, $(PROJ))
TARGET  =build
export CFLAGS TARGET
.PHONY: all clean debug msg paint release share viewer
all: $(MSGOBJ) paint viewer
clean:
	@rm -rf build debug locale msg release
	@cd paint && $(MAKE) $@
	@cd share && $(MAKE) $@
	@cd viewer && $(MAKE) $@
debug:
	@$(MAKE) TARGET=$@ "CFLAGS=$(CFLAGS) -g -DG_ENABLE_DEBUG"
release:
	@$(MAKE) TARGET=$@ "CFLAGS=$(CFLAGS) -O2 -DNDEBUG -DG_DISABLE_ASSERT -DG_DISABLE_CAST_CHECKS"
paint: share
	@cd paint && $(MAKE)
share:
	@cd share && $(MAKE)
viewer: share
	@cd viewer && $(MAKE)
$(MSG): msg.py msg.csv
	@mkdir -p msg
	@python3 $<
$(MSGOBJ): locale/%/LC_MESSAGES/msg.mo: msg/%.po
	@echo $@
	@mkdir -p $(dir $@)
	@msgfmt -o $@ $<
$(RES): %/resource.c: %.gresource.xml $(wildcard %/*.ui) $(ICONS)
	@echo $@
	@glib-compile-resources --generate-source --target $@ $<
