# Begin standard header
sp		:= $(sp).x
dirstack_$(sp)	:= $(d)
d		:= $(dir)
# End standard header

SOURCES += $(call filelist,chardecoder.cpp htmlentitysearcher.cpp htmltokenizer.cpp input_preprocessor.cpp token.cpp treeconstructor.cpp htmlparser.cpp)

GENERATOR_SOURCES := $(call filelist,htmlentitydb_generator.cpp)
GENERATOR_OBJECTS = $(addprefix $(BUILDDIR)/,$(GENERATOR_SOURCES:.cpp=.o))
GENERATOR := $(BUILDDIR)/htmlentitydb_generator
ENTITYJSON := $(call filelist,entities.json)

ENTITYDB := $(addprefix $(BUILDDIR)/,$(call filelist,htmlentitydb.cpp))
SOURCES += $(ENTITYDB)

$(ENTITYDB): $(GENERATOR) $(ENTITYJSON)
	$(GENERATOR) $(ENTITYJSON) $@

$(GENERATOR): $(GENERATOR_OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

EXTRA_CLEAN += $(GENERATOR_OBJECTS) $(GENERATOR) $(ENTITYDB)

# Begin standard footer
d		:= $(dirstack_$(sp))
sp		:= $(basename $(sp))
# End standard footer
