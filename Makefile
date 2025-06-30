# Make
BUILD  = echo $@ && $(MKDIR) && cd $@ && $(MKDIR) && $(MAKE) "OUTDIR = ../$(TARGET)" "OBJDIR = $(TARGET)"
CFLAGS = -Wall
MKDIR  = if test ! -e $(TARGET); then mkdir $(TARGET); fi
RM     = rm -fr
TARGET = build
export CC CFLAGS RM
.PHONY: all clean debug draw paint release text
all: text
clean:
	$(RM) build debug release
	cd draw  && $(MAKE) clean
	cd paint && $(MAKE) clean
	cd text  && $(MAKE) clean
debug:
	@$(MKDIR) && $(MAKE) "TARGET = $@" "CFLAGS = $(CFLAGS) -g"
release:
	@$(MKDIR) && $(MAKE) "TARGET = $@" "CFLAGS = $(CFLAGS) -O2 -DNDEBUG"
draw: text
	@$(BUILD)
paint: text
	@$(BUILD)
text:
	@$(BUILD)
