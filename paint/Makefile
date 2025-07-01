# Make: Paint
C        = $(SRCECHO) && $(CC) $(CFLAGS) -c -o $@ $<
MAKETEXT = $(OUTDIR)/text
OBJ      = $(OBJTEXT) $(OBJUSER)
OBJECHO  = echo "$@ <- $(OBJ)"
#OBJPAINT = $(addprefix $(OBJDIR)/, )
OBJTEXT  = $(addprefix $(OBJDIR)/, menu.o text.o)
OBJUSER  = $(addprefix $(OBJDIR)/, dialog.o editor.o main.o)
OBJDIR   = .
OUT      = $(OUTDIR)/paint
OUTDIR   = .
OUTTEXT  = text.c text.h
SRCECHO  = echo "$@ <- $<"
override CFLAGS += `pkg-config gtk4 --cflags`
override LIBS   += `pkg-config gtk4 --libs`
.PHONY: all clean
all: $(OUT)
clean:
	@$(RM) build debug release $(OUTTEXT)
$(OUT): $(OBJ)
	@$(OBJECHO) && $(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)
$(OBJPAINT): $(OBJDIR)/%.o: %.c paint.h
	@$(C)
$(OBJTEXT): $(OBJDIR)/%.o: %.c text.h
	@$(C)
$(OBJUSER): $(OBJDIR)/%.o: %.c paint.h text.h
	@$(C)
$(OUTTEXT): text.csv
	@$(SRCECHO) && $(MAKETEXT) $< $(OUTTEXT)
