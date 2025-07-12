# Make: Paint
C        = $(SRCECHO) && $(CC) $(CFLAGS) -c -o $@ $<
MAKETEXT = $(OUTDIR)/text
OBJ      = $(OBJINDIE) $(OBJPAINT) $(OBJTEXT) $(OBJUSER)
OBJECHO  = echo "$@ <- $(OBJ)"
OBJINDIE = $(addprefix $(OBJDIR)/, resource.o)
OBJPAINT = $(addprefix $(OBJDIR)/, canvas.o document.o layer.o)
OBJTEXT  = $(addprefix $(OBJDIR)/, menu.o text.o)
OBJUSER  = $(addprefix $(OBJDIR)/, editor.o frame.o main.o paint.o)
OBJDIR   = .
OUT      = $(OUTDIR)/paint
OUTDIR   = .
OUTRES   = resource.c
OUTTEXT  = text.c text.h
RESDIR   = ../resource
SRCECHO  = echo "$@ <- $<"
override CFLAGS += `pkg-config gtk4 --cflags`
override LIBS   += `pkg-config gtk4 --libs`
.PHONY: all clean
all: $(OUT)
clean:
	@$(RM) build debug release $(OUTRES) $(OUTTEXT)
$(OUT): $(OBJ)
	@$(OBJECHO) && $(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)
$(OBJINDIE): $(OBJDIR)/%.o: %.c
	@$(C)
$(OBJPAINT): $(OBJDIR)/%.o: %.c paint.h
	@$(C)
$(OBJTEXT): $(OBJDIR)/%.o: %.c text.h
	@$(C)
$(OBJUSER): $(OBJDIR)/%.o: %.c paint.h text.h
	@$(C)
$(OUTRES): resource.xml $(wildcard $(RESDIR)/*.png)
	@$(SRCECHO) && glib-compile-resources $< --target $@ --generate-source
$(OUTTEXT): text.csv
	@$(SRCECHO) && $(MAKETEXT) $< $(OUTTEXT)
