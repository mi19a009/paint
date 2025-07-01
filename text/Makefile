# Make: Text
OBJ     = $(OBJDIR)/main.o
OBJDIR  = .
OBJECHO = echo "$@ <- $(OBJ)"
OUT     = $(OUTDIR)/text
OUTDIR  = .
SRCECHO = echo "$@ <- $<"
.PHONY: all clean
all: $(OUT)
clean:
	@$(RM) build debug release
$(OUT): $(OBJ)
	@$(OBJECHO) && $(CC) $(CFLAGS) -o $@ $<
$(OBJ): main.c
	@$(SRCECHO) && $(CC) $(CFLAGS) -c -o $@ $<
