# Make
.PHONY: all run
all:
	@cd gtk4 && $(MAKE)
run:
	@./gtk4/obj/paint
