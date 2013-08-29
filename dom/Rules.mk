# Begin standard header
sp		:= $(sp).x
dirstack_$(sp)	:= $(d)
d		:= $(dir)
# End standard header

SOURCES += $(call filelist,node.cpp element.cpp document.cpp text.cpp exception.cpp htmlelement.cpp graphics.cpp)

# Begin standard footer
d		:= $(dirstack_$(sp))
sp		:= $(basename $(sp))
# End standard footer
