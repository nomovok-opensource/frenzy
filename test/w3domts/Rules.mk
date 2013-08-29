# Begin standard header
sp		:= $(sp).x
dirstack_$(sp)	:= $(d)
d		:= $(dir)
# End standard header

AUTOGENSOURCES := $(addprefix $(BUILDDIR)/,$(call filelist, level1core.cpp))

XSL := $(d)/test-to-frenzy.xsl

$(BUILDDIR)/$(d)/%.cpp: $(d)/xml/%.xml $(XSL)
	@mkdir -p $(dir $@)
	xalan -in $< -out $@ -xsl $(XSL)

TESTER_SOURCES += $(AUTOGENSOURCES)
EXTRA_CLEAN += $(AUTOGENSOURCES)

# Begin standard footer
d		:= $(dirstack_$(sp))
sp		:= $(basename $(sp))
# End standard footer
