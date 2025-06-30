# Make: Paint
C        = echo $@ && $(CC) $(CFLAGS) -c -o $@ $<
MAKETEXT = $(OUTDIR)/text
OBJTEXT  = $(addprefix $(OBJDIR)/, text.o)
OBJDIR   = .
OUTDIR   = .
OUTTEXT  = text.c text.h
override CFLAGS += `pkg-config gtk4 --cflags`
override LIBS   += `pkg-config gtk4 --libs`
.PHONY: all clean
all: $(OBJTEXT)
clean:
	$(RM) build debug release $(OUTTEXT)
$(OBJTEXT): $(OBJDIR)/%.o: %.c text.h
	@$(C)
$(OUTTEXT): text.csv
	@echo $@ && $(MAKETEXT) $< $(OUTTEXT)
