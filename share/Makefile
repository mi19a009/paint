# Make Shared Library
override CFLAGS +=$(shell pkg-config gtk4 --cflags) -Wall
OUT              =$(OUTDIR)/libshare.a
OUTDIR           =../$(TARGET)
SRC              =share.c
SRCOBJ           =$(addprefix $(TARGET)/, $(SRC:%.c=%.o))
TARGET           =build
.PHONY: all clean
all: $(OUT)
clean:
	@rm -rf build debug release
$(OUT): $(SRCOBJ)
	@echo $@
	@mkdir -p $(OUTDIR)
	@ar rcs $@ $(SRCOBJ)
	@touch $@
$(SRCOBJ): $(TARGET)/%.o: %.c share.h
	@echo $@
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
