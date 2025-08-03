# Make GTK Application
NAME     := paint
RESOURCE := src/resource.c
RM       := rm -fr
TARGET   := build
OBJS     := \
	main.o \
	paintapplication.o \
	paintapplicationwindow.o
CFLAGS   := `pkg-config gtk4 --cflags` -I. -Wall
LIBS     := `pkg-config gtk4 --libs`
INCLUDE  := paint.h
UI       := $(wildcard gtk/*.ui) $(wildcard src/*.ui)
ICONS    := $(wildcard icons/*.png)
GTK      = $(addprefix $(TARGET)/, resource.o)
PAINT    = $(addprefix $(TARGET)/, $(OBJS))
INPUT    = $(GTK) $(PAINT)
OUTPUT   = $(TARGET)/$(NAME)
.PHONY: all clean debug release resource target
all: target $(OUTPUT)
resource: $(RESOURCE)
debug: TARGET = debug
debug: CFLAGS += -g
debug: all
release: TARGET = release
release: CFLAGS += -O2 -DNDEBUG
release: all
clean:
	@$(RM) build debug release
target:
	@if test ! -e $(TARGET); then mkdir $(TARGET); fi
$(OUTPUT): $(INPUT)
	@echo "$@" && $(CC) $(CFLAGS) -o $@ $(INPUT) $(LIBS)
$(GTK): $(TARGET)/%.o: src/%.c
	@echo "$@ <- $<" && $(CC) $(CFLAGS) -c -o $@ $<
$(PAINT): $(TARGET)/%.o: src/%.c $(INCLUDE)
	@echo "$@ <- $<" && $(CC) $(CFLAGS) -c -o $@ $<
$(RESOURCE): $(NAME).xml $(ICONS) $(UI)
	@echo "$@" && glib-compile-resources $< --target $@ --generate-source
