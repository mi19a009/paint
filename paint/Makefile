# Make: Paint
C        = $(SRCECHO) && $(CC) $(CFLAGS) -c -o $@ $<
MAKETEXT = $(OUTDIR)/text
OBJ      = $(OBJIND) $(OBJTEXT) $(OBJUSER)
OBJECHO  = echo "$@ <- $(OBJ)"
OBJIND   = $(addprefix $(OBJDIR)/, resource.o)
#OBJPAINT = $(addprefix $(OBJDIR)/, )
OBJTEXT  = $(addprefix $(OBJDIR)/, menu.o text.o)
OBJUSER  = $(addprefix $(OBJDIR)/, dialog.o editor.o main.o)
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
$(OBJIND): $(OBJDIR)/%.o: %.c
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
