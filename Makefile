CC:=g++

# General-purpuse compiler flags
CFLAGS:=-std=c++2a -ggdb3 -fPIE -pie $(CMACHINE) $(CWARN)

# Warning flags
CWARN:=-Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat\
-Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts\
-Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body\
-Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness\
-Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd\
-Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls\
-Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel\
-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods\
-Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum\
-Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast\
-Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers\
-Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector\
-Wlarger-than=8192 -Wstack-usage=8192

# Debug sanitizer flags
CDEBUG:=-D _DEBUG -fcheck-new -fsized-deallocation -fstack-protector\
-fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer\
-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,${strip \
}float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,${strip \
}null,object-size,return,returns-nonnull-attribute,shift,${strip \
}signed-integer-overflow,undefined,unreachable,vla-bound,vptr

# Machine-specific flags
CMACHINE:=-mavx512f

BUILDTYPE?=Debug

# Select optimization level and sanitizer based on BUILDTYPE
ifeq ($(BUILDTYPE), Release)
	CFLAGS:=-O3 $(CFLAGS)
else
	CFLAGS:=-O0 $(CDEBUG) $(CFLAGS)
endif

PROJECT	:= tlc
VERSION := 0.0.1

SRCDIR	:= src
TESTDIR := tests
LIBDIR	:= lib
INCDIR	:= include

BUILDDIR:= build
OBJDIR 	:= $(BUILDDIR)/obj
BINDIR	:= $(BUILDDIR)/bin

SRCEXT	:= cpp
HEADEXT	:= h
OBJEXT	:= o

# Name of test binary
TEST_BIN_NAME:=$(PROJECT)_tests

# Find all source files
SOURCES := $(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")
# Select all 'main.cpp's
MAIN_SRCS := $(filter %/main.$(SRCEXT), $(SOURCES))
# Determine the suffixes of executables
EXE_SUFFIX := $(patsubst $(SRCDIR)/$(PROJECT)/%, %, $(MAIN_SRCS:/main.$(SRCEXT)=))

# Executable file names
EXECUTABLES := $(patsubst %, $(BINDIR)/$(PROJECT)_%,$(EXE_SUFFIX))

# Exclude 'main.cpp's from object list
SOURCES := $(filter-out $(MAIN_SRCS), $(SOURCES))
# Common object files
OBJECTS	:= $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

# Test source files
TESTSRCS:= $(shell find $(TESTDIR) -type f -name "*$(SRCEXT)")
# Test object files
TESTOBJS:= $(patsubst %,$(OBJDIR)/%,$(TESTSRCS:.$(SRCEXT)=.$(OBJEXT)))
# Static libraries
LIBS	:= $(patsubst $(LIBDIR)/lib%.a, %, $(shell find $(LIBDIR) -type f))

# Include compiler flags
INCFLAGS:= -I$(SRCDIR) -I$(INCDIR)
# Linker flags
LFLAGS  := -Llib/ $(addprefix -l, $(LIBS))

ARGS?=--help

all: $(EXECUTABLES)

remake: cleaner all

init:
	@mkdir -p $(SRCDIR)
	@mkdir -p $(TESTDIR)
	@mkdir -p $(INCDIR)
	@mkdir -p $(LIBDIR)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

# Collect source objects
$(OBJDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@echo Collecting $@
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $@

# Build project binaries
$(BINDIR)/$(PROJECT)_%: $(OBJECTS) $(OBJDIR)/$(PROJECT)/%/main.$(OBJEXT)
	@echo Building $@
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

# Collect test objects
$(OBJDIR)/$(TESTDIR)/%.$(OBJEXT): $(TESTDIR)/%.$(SRCEXT)
	@echo Collecting $@
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCFLAGS) -I$(TESTDIR) -c $< -o $@

# Build test binary
$(BINDIR)/$(TEST_BIN_NAME): $(filter-out %/main.$(OBJEXT),$(OBJECTS)) $(TESTOBJS)
	@echo Building $@
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

clean:
	@rm -rf $(OBJDIR)

cleaner: clean
	@rm -rf $(BINDIR)

run_%: $(BINDIR)/$(PROJECT)_%
	@$< $(ARGS)

test: $(TEST_BIN_NAME)
	@$< $(ARGS)

.PHONY: all remake clean cleaner

