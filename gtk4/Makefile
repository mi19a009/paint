# Make GTK4 Application
CFLAGS := `pkg-config gtk4 --cflags` -Wall
LIBS   := `pkg-config gtk4 --libs`
OBJ    := obj
RM     := rm -rf
GTK = \
	$(OBJ)/main.o \
	$(OBJ)/resource.o
PAINT = \
	$(OBJ)/paint.o \
	$(OBJ)/paint-application.o \
	$(OBJ)/paint-window.o
.PHONY: all clean debug release target
all: target $(OBJ)/paint
debug: OBJ = debug
debug: CFLAGS += -g
debug: all
release: OBJ = release
release: CFLAGS += -O2 -DNDEBUG
release: all
clean:
	@$(RM) debug obj release resource.c resource.h
target:
	@echo "$(OBJ)" && if test ! -e $(OBJ); then mkdir $(OBJ); fi
$(OBJ)/paint: $(GTK) $(PAINT)
	@echo "$@" && $(CC) $(CFLAGS) -o $@ $(GTK) $(PAINT) $(LIBS)
$(OBJ)/gschemas.compiled:
	@echo "$@" && glib-compile-schemas --targetdir=$(OBJ) .
$(GTK): $(OBJ)/%.o: %.c
	@echo "$@ <- $<" && $(CC) $(CFLAGS) -c -o $@ $<
$(PAINT): $(OBJ)/%.o: %.c paint.h
	@echo "$@ <- $<" && $(CC) $(CFLAGS) -c -o $@ $<
resource.c: paint.gresource.xml $(wildcard *.ui) $(wildcard gtk/*.ui) $(wildcard icons/16x16/actions/*.*)
	@echo "$@" && glib-compile-resources $< --target $@ --generate-source
resource.h: paint.gresource.xml
	@echo "$@" && glib-compile-resources $< --target $@ --generate-header
