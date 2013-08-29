# Begin standard header
sp		:= $(sp).x
dirstack_$(sp)	:= $(d)
d		:= $(dir)
# End standard header

# Main tester machinery
TESTER_SOURCES += $(call filelist,tester.cpp test_helpers.cpp)

# Test case files
TESTER_SOURCES += $(call filelist,test_htmlentitysearcher.cpp test_htmltokenizer.cpp test_preprocessor.cpp test_treeconstructor.cpp test_unicode.cpp test_utf8_decoder.cpp test_dom.cpp test_vector.cpp test_htmlparser.cpp)

dir := $(d)/w3domts
include $(dir)/Rules.mk

# Begin standard footer
d		:= $(dirstack_$(sp))
sp		:= $(basename $(sp))
# End standard footer
