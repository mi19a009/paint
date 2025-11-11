# Make Viewer
override CFLAGS +=$(shell pkg-config gtk4 --cflags) -Wall -I../share
LIBS             =$(shell pkg-config gtk4 --libs) -L../$(TARGET) -lshare
OUT              =$(OUTDIR)/viewer
OUTDIR           =../$(TARGET)
RES              =resource.c
RESOBJ           =$(addprefix $(TARGET)/, $(RES:%.c=%.o))
SRC              =app.c document.c viewer.c
SRCOBJ           =$(addprefix $(TARGET)/, $(SRC:%.c=%.o))
TARGET           =build
.PHONY: all clean
all: $(OUT)
clean:
	@rm -rf build debug release
	@$(RM) resource.c
$(OUT): $(RESOBJ) $(SRCOBJ)
	@echo $@
	@mkdir -p $(OUTDIR)
	@$(CC) $(CFLAGS) -o $@ $(RESOBJ) $(SRCOBJ) $(LIBS)
$(RESOBJ): $(TARGET)/%.o: %.c
	@echo $@
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
$(SRCOBJ): $(TARGET)/%.o: %.c viewer.h
	@echo $@
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
resource.c:
	@cd .. && $(MAKE) viewer/$@
