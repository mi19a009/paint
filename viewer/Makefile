# Make Viewer
ASSETS           =$(wildcard *.ui) $(wildcard ../icons/48x48/actions/*.png)
override CFLAGS +=-Wall $(shell pkg-config gtk4 --cflags) -I../share
LIBS             =-lshare $(shell pkg-config gtk4 --libs) -L../$(TARGET)
NAME             =com.github.mi19a009.viewer
OUT              =$(OUTDIR)/viewer
OUTDIR           =../$(TARGET)
RES              =resource.c
RESOBJ           =$(addprefix $(TARGET)/, $(RES:%.c=%.o))
SCHEMA           =$(SCHEMAS)/$(NAME).gschema.xml
SCHEMAS          =$(HOME)/.local/share/glib-2.0/schemas
SHARE            =$(OUTDIR)/libshare.a
SRC              =app.c document.c main.c
SRCOBJ           =$(addprefix $(TARGET)/, $(SRC:%.c=%.o))
TARGET           =build
.PHONY: all clean install uninstall
all: $(OUT) $(SCHEMA)
clean:
	@rm -rf build debug release
	@$(RM) resource.c
uninstall:
	@$(RM) $(SCHEMA)
	@glib-compile-schemas $(SCHEMAS)
$(OUT): $(RESOBJ) $(SRCOBJ) $(SHARE)
	@echo "$(PWD)/$@"
	@mkdir -p $(OUTDIR)
	@$(CC) $(CFLAGS) -o $@ $(RESOBJ) $(SRCOBJ) $(LIBS)
$(RESOBJ): $(TARGET)/%.o: %.c
	@echo "$(PWD)/$@"
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
$(SRCOBJ): $(TARGET)/%.o: %.c viewer.h ../share/share.h
	@echo "$(PWD)/$@"
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
resource.c: viewer.gresource.xml $(ASSETS)
	@echo "$(PWD)/$@"
	@cd .. && glib-compile-resources --generate-source --target viewer/$@ viewer/$<
viewer.gschema.valid: viewer.gschema.xml
	@echo "$(PWD)/$@"
	@glib-compile-schemas --strict --dry-run --schema-file=$<
	@touch $@
$(SCHEMA): viewer.gschema.xml viewer.gschema.valid
	@echo "$@"
	@mkdir -p $(SCHEMAS)
	@cp $< $@
	@glib-compile-schemas $(SCHEMAS)
