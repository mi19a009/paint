# Make GTK4 Application
CFLAGS := `pkg-config gtk4 --cflags` -I. -Wall
LIBS   := `pkg-config gtk4 --libs`
OBJ    := obj
RM     := rm -rf
SRC    := src
GTK     = $(OBJ)/resource.o
PAINT   = \
	$(OBJ)/app.o \
	$(OBJ)/document.o \
	$(OBJ)/paint.o
.PHONY: all clean debug release target
all: target $(OBJ)/paint
debug: OBJ = debug
debug: CFLAGS += -g
debug: all
release: OBJ = release
release: CFLAGS += -O2 -DNDEBUG
release: all
clean:
	@$(RM) debug obj release
target:
	@echo "$(OBJ)" && if test ! -e $(OBJ); then mkdir $(OBJ); fi
$(OBJ)/paint: $(GTK) $(PAINT)
	@echo "$@" && $(CC) $(CFLAGS) -o $@ $(GTK) $(PAINT) $(LIBS)
$(GTK): $(OBJ)/%.o: $(SRC)/%.c
	@echo "$@ <- $<" && $(CC) $(CFLAGS) -c -o $@ $<
$(PAINT): $(OBJ)/%.o: $(SRC)/%.c paint.h
	@echo "$@ <- $<" && $(CC) $(CFLAGS) -c -o $@ $<
$(SRC)/resource.c: paint.xml $(wildcard gtk/*.*) $(wildcard icons/16x16/actions/*.*)
	@echo "$@" && glib-compile-resources $< --target $@ --generate-source
resource.h: paint.xml
	@echo "$@" && glib-compile-resources $< --target $@ --generate-header
$(OBJ)/gschemas.compiled: $(wildcard settings/*.*)
	@echo "$@" && glib-compile-schemas --targetdir=$(OBJ) settings
