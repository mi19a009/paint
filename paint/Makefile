# Make Paint
ASSETS           =$(wildcard *.ui) $(wildcard ../icons/48x48/actions/*.png)
override CFLAGS +=$(shell pkg-config gtk4 --cflags) -Wall -I../share
LIBS             =$(shell pkg-config gtk4 --libs) -L../$(TARGET) -lshare
OUT              =$(OUTDIR)/paint
OUTDIR           =../$(TARGET)
RES              =resource.c
RESOBJ           =$(addprefix $(TARGET)/, $(RES:%.c=%.o))
SRC              =app.c document.c paint.c
SRCOBJ           =$(addprefix $(TARGET)/, $(SRC:%.c=%.o))
TARGET           =build
.PHONY: all clean
all: $(OUT)
clean:
	@rm -rf build debug release
	@$(RM) resource.c
$(OUT): $(RESOBJ) $(SRCOBJ)
	@echo "$(PWD)/$@"
	@mkdir -p $(OUTDIR)
	@$(CC) $(CFLAGS) -o $@ $(RESOBJ) $(SRCOBJ) $(LIBS)
$(RESOBJ): $(TARGET)/%.o: %.c
	@echo "$(PWD)/$@"
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
$(SRCOBJ): $(TARGET)/%.o: %.c paint.h
	@echo "$(PWD)/$@"
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
resource.c: paint.gresource.xml $(ASSETS)
	@echo "$(PWD)/$@"
	@cd .. && glib-compile-resources --generate-source --target paint/$@ paint/$<
