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
	@if test ! -e $(OBJ); then mkdir $(OBJ); fi
$(OBJ)/paint: $(PAINT)
	@$(CC) $(CFLAGS) -o $@ $(PAINT) $(LIBS)
$(PAINT): $(OBJ)/%.o: $(SRC)/%.c paint.h
	@$(CC) $(CFLAGS) -c -o $@ $<
