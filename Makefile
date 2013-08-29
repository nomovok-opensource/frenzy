CXX ?= g++
CXXFLAGS ?= -g -O2 -Wall -Wextra

# Variables modified by subdirs
SOURCES :=
TESTER_SOURCES :=
DEPS :=
EXTRA_CLEAN :=
INCLUDES :=

BUILDDIR := build

FRENZYLIB = libfrenzy.a
FRENZYBIN = frenzy

# all-rule needs to be the first rule for default-goal magickry to match it
all: $(FRENZYBIN)

# Function that expands to the list of given files with the path from $(d) prepended
filelist = $(foreach filename,$(1),$(d)/$(filename))

# Fetch values from subdirs
dir := dom
include dom/Rules.mk
dir := parser
include parser/Rules.mk
dir := test
include test/Rules.mk
dir := util
include util/Rules.mk

WRATHCONFIG := $(WRATH_SOURCES)/wrath-config

OBJECTS = $(subst build/build,build,$(addprefix $(BUILDDIR)/,$(SOURCES:.cpp=.o)))
TESTER_OBJECTS = $(subst build/build,build,$(addprefix $(BUILDDIR)/,$(TESTER_SOURCES:.cpp=.o)))

FRENZY_SOURCES = main.cpp
FRENZY_OBJECTS = $(addprefix $(BUILDDIR)/,$(FRENZY_SOURCES:.cpp=.o))
FRENZY_DEPS = $(FRENZY_OBJECTS:.o=.d)

DEPS += $(OBJECTS:.o=.d) $(TESTER_OBJECTS:.o=.d) $(FRENZY_DEPS)

WRATHLIB := $(WRATH_SOURCES)/release/libwrath_release.a

LDFLAGS = $(CXXFLAGS)
LIBS = $(shell $(WRATHCONFIG) --release --static --libs)
TESTER_LIBS = -lboost_unit_test_framework

CXXFLAGS += -I.

$(FRENZY_OBJECTS) $(FRENZY_DEPS): CXXFLAGS += $(shell $(WRATHCONFIG) --release --static --cflags)

$(FRENZY_OBJECTS) $(FRENZY_DEPS): $(WRATHLIB)

clean:
	rm -f $(DEPS) $(OBJECTS) $(TESTER_OBJECTS) $(FRENZY_OBJECTS) tester $(FRENZYLIB) $(FRENZYBIN) $(EXTRA_CLEAN)
	@[ -d $(BUILDDIR) ] && find $(BUILDDIR) -depth -type d -exec rmdir --ignore-fail-on-non-empty "{}" ";" || true

$(FRENZYBIN): $(FRENZY_OBJECTS) $(FRENZYLIB) $(WRATHLIB)
	$(CXX) $(LDFLAGS) $(FRENZY_OBJECTS) $(FRENZYLIB) -o $@ $(LIBS)

$(FRENZYLIB): $(OBJECTS)
	ar rcs $@ $^

$(WRATHLIB): FORCE
	$(MAKE) -C $(WRATH_SOURCES) wrath-lib-static

tester: $(TESTER_OBJECTS) $(FRENZYLIB)
	$(CXX) $(LDFLAGS) $^ -o $@ $(TESTER_LIBS)

build/%.d: %.cpp
	@echo Generating "$@"...
	@mkdir -p $(dir $@)
	@./makedepend.sh "$(CXX)" "$(CXXFLAGS)" build "$*" "$<" "$@"

build/%.d: build/%.cpp
	@echo Generating "$@"...
	@mkdir -p $(dir $@)
	@./makedepend.sh "$(CXX)" "$(CXXFLAGS)" build "$*" "$<" "$@"

build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: build/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: tester
	@./tester --report_level=short

.PHONY: all clean test FORCE

FORCE:

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
