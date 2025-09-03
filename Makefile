# Make
.PHONY: all clean run
all:
	@cd gtk4 && $(MAKE)
clean:
	@cd gtk4 && $(MAKE) clean
run:
	@./gtk4/obj/paint
