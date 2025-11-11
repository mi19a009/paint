# Make Messages
LINGUAS =ja
MOS     =$(addprefix ../locale/, $(addsuffix /LC_MESSAGES/msg.mo, $(LINGUAS)))
MSG     =msg.py msg.csv
POS     =$(addprefix $(POSDIR)/, $(addsuffix .po, $(LINGUAS)))
POSDIR  =locale
.PHONY: all clean
all: $(MOS)
clean:
	@rm -rf locale
$(MOS): ../locale/%/LC_MESSAGES/msg.mo: locale/%.po
	@echo "$(PWD)/$@"
	@mkdir -p $(dir $@)
	@msgfmt -o $@ $<
$(POS): $(MSG)
	@echo "$(PWD)/$@"
	@mkdir -p $(POSDIR)
	@python3 $(MSG) $(notdir $(basename $@))
