#!/usr/bin/make

CFLAGS   := -Wall
GIOFLAGS := `pkg-config gio-2.0 --cflags`
GTKFLAGS := `pkg-config gtk4 --cflags`
LIBS     := -lm `pkg-config gtk4 --libs`
OBJS     := paint.o paintapplication.o paintapplicationwindow.o paintsettings.o resource.o text.o
OPT      := -g
OUT      := debug
CCC    = echo $@; if test ! -e $(OUT); then mkdir $(OUT); fi; $(CC) -o $@ -c $< $(OPT) $(CFLAGS)
CCCGIO = $(CCC) $(GIOFLAGS)
CCCGTK = $(CCC) $(GTKFLAGS)
.PHONY: all clean debug release

# Debug Build
all: $(OUT)/paint

# Clean Build
clean:
	@rm -fr debug release

# Release Build
release:
	@make "OUT := release" "OPT := -O2"

# Paint
$(OUT)/paint: $(addprefix $(OUT)/, $(OBJS))
	@echo $@; $(CC) -o $@ $^ $(OPT) $(CFLAGS) $(GTKFLAGS) $(LIBS)

# Paint OBJ
$(OUT)/paint.o: paint.c paint.h text.h
	@$(CCCGTK)

# Paint OBJ%
$(OUT)/paint%.o: paint%.c paint.h text.h
	@$(CCCGTK)

# Resource C
$(OUT)/resource.c: resource.xml $(wildcard resource/*.*)
	@echo $@; glib-compile-resources $< --target $@ --generate-source

# Resource OBJ
$(OUT)/resource.o: $(OUT)/resource.c
	@$(CCCGIO)

# Text OBJ
$(OUT)/text.o: text.c text.h
	@$(CCC)
