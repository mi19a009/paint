# Makefile
INC = -I/usr/include/gtk-3.0 -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/fribidi -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/x86_64-linux-gnu -I/usr/include/webp -I/usr/include/gio-unix-2.0 -I/usr/include/atk-1.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -pthread
LIB = -lgtk-3 -lgdk-3 -lz -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0
OBJ = main.o resource.o
OUT = a.out
SRC = resource.c

.PHONY: all clean

all: $(OUT)

clean:
	@$(RM) $(OBJ) $(OUT) $(SRC)

debug:
	make "CFLAGS = -Wall -g -D_DEBUG"

release:
	make "CFLAGS = -Wall -O2"

$(OUT): $(OBJ)
	@$(CC) -o $(OUT) $(OBJ) $(CFLAGS) $(LIB)

main.o: main.c
	@$(CC) -c main.c $(CFLAGS) $(INC)

resource.c: resource.xml resource/new.svg resource/open.svg resource/save.svg
	@glib-compile-resources resource.xml --target resource.c --generate-source

resource.o: resource.c
	@$(CC) -c resource.c $(CFLAGS) $(INC)
