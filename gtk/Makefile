# Makefile
RM     := rm -fr
CFLAGS := $(OPTION) `pkg-config gtk+-3.0 --cflags`
INC    := -I.
LIBS   := -lm `pkg-config gtk+-3.0 --libs`
RESDIR := resource/
SRCDIR := source/
OBJ    := $(addprefix $(OUTDIR), about.o app.o main.o menu.o new.o resource.o status.o toolbar.o)
OUT    := $(OUTDIR)a.out
RES    := $(wildcard $(RESDIR)*.*)
RESOBJ := $(OUTDIR)resource.o
RESSRC := $(OUTDIR)resource.c
RESXML := resource.xml
SRC    := resource.c

.PHONY: all clean debug release
all: $(OUT)

clean:
	@$(RM) debug release

debug:
	@if test ! -e debug; then mkdir debug; fi
	@make "OUTDIR := debug/" "OPTION := -Wall -g -D_DEBUG"

release:
	@if test ! -e release; then mkdir release; fi
	@make "OUTDIR := release/" "OPTION := -Wall -O2"

$(OUT): $(OBJ)
	@echo $@
	@$(CC) -o $(OUT) $(OBJ) $(CFLAGS) $(LIBS)

$(RESSRC): $(RESXML) $(RES)
	@echo $@
	@glib-compile-resources $< --target $@ --generate-source

$(RESOBJ): $(RESSRC)
	@echo $@
	@$(CC) -o $@ -c $< $(CFLAGS) $(INC)

$(OUTDIR)%.o: $(SRCDIR)%.c
	@echo $@
	@$(CC) -o $@ -c $< $(CFLAGS) $(INC)
