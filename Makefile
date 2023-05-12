CC:=g++

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

CDEBUG:=-D _DEBUG -ggdb3 -fcheck-new -fsized-deallocation -fstack-protector\
-fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer\
-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,${strip \
}float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,${strip \
}null,object-size,return,returns-nonnull-attribute,shift,${strip \
}signed-integer-overflow,undefined,unreachable,vla-bound,vptr

CMACHINE:=-mavx512f

CFLAGS:=-std=c++2a -fPIE -pie $(CMACHINE) $(CWARN)
BUILDTYPE?=Debug

ifeq ($(BUILDTYPE), Release)
	CFLAGS:=-O3 $(CFLAGS)
else
	CFLAGS:=-O0 $(CDEBUG) $(CFLAGS)
endif

PROJECT	:= project
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


SOURCES := $(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")
TESTS	:= $(shell find $(TESTDIR) -type f -name "*$(SRCEXT)")
LIBS	:= $(patsubst lib%.a, %, $(shell find $(LIBDIR) -type f))
OBJECTS	:= $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))
TESTOBJS:= $(patsubst %,$(OBJDIR)/%,$(TESTS:.$(SRCEXT)=.$(OBJEXT)))

INCFLAGS:= -I$(SRCDIR) -I$(INCDIR)
LFLAGS  := -Llib/ $(addprefix -l, $(LIBS))

all: $(BINDIR)/$(PROJECT)

remake: cleaner all

init:
	@mkdir -p $(SRCDIR)
	@mkdir -p $(INCDIR)
	@mkdir -p $(LIBDIR)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

build_lib: $(OBJECTS)
	@mkdir -p dist/include
	@mkdir -p dist/lib
	@ar rcs dist/lib/lib$(PROJECT).a $^
	@find $(SRCDIR) -type f -name *.$(HEADEXT) -exec\
		bash -c 'cp -p --parents {} dist/include' \;
	@tar -czf dist/$(PROJECT)-$(VERSION)-linux-x86_64.tar.gz dist/*
	@rm -r dist/include
	@rm -r dist/lib

# Build test objects
$(OBJDIR)/$(TESTDIR)/%.$(OBJEXT): $(TESTDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCFLAGS) -I$(TESTDIR) -c $< -o $@

# Build source objects
$(OBJDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $@

# Build project binary
$(BINDIR)/$(PROJECT): $(OBJECTS)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $^ $(LFLAGS) -o $(BINDIR)/$(PROJECT)

# Build test binary
$(BINDIR)/$(PROJECT)_tests: $(filter-out %/main.o,$(OBJECTS)) $(TESTOBJS)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $^ $(LFLAGS) -o $(BINDIR)/$(PROJECT)_tests

clean:
	@rm -rf $(OBJDIR)

cleaner: clean
	@rm -rf $(BINDIR)

run: $(BINDIR)/$(PROJECT)
	$(BINDIR)/$(PROJECT) $(ARGS)

test: $(BINDIR)/$(PROJECT)_tests
	 $(BINDIR)/$(PROJECT)_tests $(ARGS)

.PHONY: all remake clean cleaner

