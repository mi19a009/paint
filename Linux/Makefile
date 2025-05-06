# Makefile
TARGET   := paint
OBJS     := about.o application.o document.o main.o resource.o
HEADER   := paint.h

# Directories
IMGDIR   := Image
OUTDIR   := Debug
RESDIR   := Resource

# Options
CFLAGS   := `pkg-config gtk4 --cflags`
LIBS     := -lm `pkg-config gtk4 --libs`

# Commands
MKOUTDIR := if test ! -e $(OUTDIR); then mkdir $(OUTDIR); fi
RC       := glib-compile-resources
RM       := rm -fr

.PHONY: all cflags clean debug libs release
all: $(OUTDIR)/$(TARGET)
cflags:
	@echo $(CFLAGS)
clean:
	@$(RM) Debug Release resource.c
debug:
	@make "OUTDIR := Debug" "OPTION := -Wall -g"
libs:
	@echo $(LIBS)
release:
	@make "OUTDIR := Release" "OPTION := -Wall -O2"
$(OUTDIR)/$(TARGET): $(addprefix $(OUTDIR)/, $(OBJS))
	@echo $@
	@$(CC) -o $@ $^ $(OPTION) $(CFLAGS) $(LIBS)
resource.c: resource.xml $(wildcard $(IMGDIR)/*.*) $(wildcard $(RESDIR)/*.*)
	@echo $@
	@$(RC) $< --target $@ --generate-source
$(OUTDIR)/resource.o: resource.c
	@echo $@
	@$(MKOUTDIR)
	@$(CC) -o $@ -c $< $(OPTION) $(CFLAGS)
$(OUTDIR)/%.o: %.c $(HEADER)
	@echo $@
	@$(MKOUTDIR)
	@$(CC) -o $@ -c $< $(OPTION) $(CFLAGS)
