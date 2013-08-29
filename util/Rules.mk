# Begin standard header
sp		:= $(sp).x
dirstack_$(sp)	:= $(d)
d		:= $(dir)
# End standard header

SOURCES += $(call filelist,unicode.cpp vector.cpp)

# Begin standard footer
d		:= $(dirstack_$(sp))
sp		:= $(basename $(sp))
# End standard footer
