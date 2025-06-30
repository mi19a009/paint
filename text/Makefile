# Make: Text
OUT    = $(OUTDIR)/text
OUTDIR = .
.PHONY: all clean
all: $(OUT)
clean:
	$(RM) build debug release
$(OUT): main.c
	@echo $@ && $(CC) $(CFLAGS) -o $@ $<
