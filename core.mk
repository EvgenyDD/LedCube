#######################################
#           VERSION 2.1
# #####       Manual:             #####
#
# 	EXE_NAME=my_prj
# 	SOURCES += foo.c/xx/pp
# 	SOURCES += $(wildcard, path/.c)
# 	SOURCES += $(call rwildcard,.,*.cpp)
# 	INCDIR += $(dir ($(call rwildcard,.,*.h)))
# 	MAKE_EXECUTABLE=yes
# 	MAKE_BINARY=no
# 	MAKE_SHARED_LIB=yes
# 	MAKE_STATIC_LIB=yes
#   PPDEFS=STM32 VAR0=12
# 
# 	BUILDDIR = build
# 
#   EXT_LIBS+=setupapi
#   LIBDIR+=
#
# 	FOREIGN_MAKE_TARGETS=extlib/build/libext.so
#
# #####       Other: 		      #####
# 	TCHAIN = x86_64-w64-mingw32-
#   LDFLAGS += -static (on win c++ mingw)
# 	CREATE_MAP=yes
# 	CREATE_LST=yes
# 	COLORIZE=yes
# 	VERBOSE=yes
#	.prebuild:
#	$(SOURCES): dependencies
#######################################

#######################################
# build tool's names
#######################################
CC=$(TCHAIN)gcc
CXX=$(TCHAIN)g++
OC=$(TCHAIN)objcopy
AR=$(TCHAIN)ar
RL=$(TCHAIN)ranlib
LST=$(TCHAIN)objdump
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

#######################################
# color & verbose settings
#######################################
ifeq ($(strip $(COLORIZE)),yes)
	CLRED=\e[31m
	CLGRN=\e[32m
	CLYEL=\e[33m
	CLRST=\e[0m
	VECHO_=printf
else
	CLRED=
	CLGRN=
	CLYEL=
	CLRST=
	VECHO_=printf
endif

ifneq ($(VERBOSE),yes)
	Q:=@
	VECHO=@$(VECHO_)
else
	Q:= 
	VECHO=@true
endif

#######################################
# manage output files suffixes
#######################################
ifeq ($(SHARED_LIB_EXT),)
	SHARED_LIB_EXT=.so
else
	SHARED_LIB_EXT:=.$(SHARED_LIB_EXT)
endif

ifeq ($(STATIC_LIB_EXT),)
	STATIC_LIB_EXT=.a
else
	STATIC_LIB_EXT:=.$(STATIC_LIB_EXT)
endif

ifneq ($(EXE_EXT),)
	EXE_EXT:=.$(EXE_EXT)
endif

ifeq ($(EXE_NAME),)
	EXE_NAME=program
endif

ifeq ($(BUILDDIR),)
	BUILDDIR=build
endif

ifeq ($(MAKE_BINARY),)
	ifeq ($(MAKE_EXECUTABLE),)
		ifeq ($(MAKE_SHARED_LIB),)
			ifeq ($(MAKE_STATIC_LIB),)
				MAKE_EXECUTABLE:=yes
			endif
		endif
	endif
endif

OBJDIR:=$(BUILDDIR)/obj
UPDIR:=_updir_

EXT_OBJECTS+=$(FOREIGN_MAKE_TARGETS)

SELFDEP +=Makefile

#######################################
# C/C++ flags tuning
#######################################
FLAGS=-c -Wall

ifneq ($(CDIALECT),)
FLAGS+=-O$(OPT_LVL)
endif

FLAGS+=$(DBG_OPTS)
FLAGS+=-MMD -MP 
FLAGS+=$(MCPU) $(addprefix -I,$(INCDIR)) $(addprefix -D,$(PPDEFS)) 

ifneq ($(CDIALECT),)
CFLAGS+=-std=$(CDIALECT)
endif

ifneq ($(CXXDIALECT),)
CXXFLAGS+=-std=$(CXXDIALECT)
endif

LDFLAGS+=$(MCPU) 
ifeq ($(strip $(CREATE_MAP)),yes)
LDFLAGS+=-Wl,-Map=$(BUILDDIR)/$(EXE_NAME).map
endif
LDFLAGS+=$(addprefix -T,$(LDSCRIPT))

#######################################
# sort sources & generate object names
#######################################
INCDIR:=$(sort $(foreach _HEADERS, $(INCDIR), $(_HEADERS)))

CXX_SOURCES:=$(filter %.cpp %.cxx %.c++ %.cc %.C, $(SOURCES))
CXX_OBJECTS:=$(CXX_SOURCES:.cpp=.o)
CXX_OBJECTS:=$(CXX_OBJECTS:.cxx=.o)
CXX_OBJECTS:=$(CXX_OBJECTS:.c++=.o)
CXX_OBJECTS:=$(CXX_OBJECTS:.cc=.o)
CXX_OBJECTS:=$(CXX_OBJECTS:.C=.o)

C_SOURCES:=$(filter %.c, $(SOURCES))
C_OBJECTS:=$(C_SOURCES:.c=.o)

S_SOURCES:=$(filter %.s %.S, $(SOURCES))
S_OBJECTS:=$(S_SOURCES:.s=.o)
S_OBJECTS:=$(S_OBJECTS:.S=.o)

CXX_OBJECTS:=$(subst ..,$(UPDIR),$(addprefix $(OBJDIR)/, $(CXX_OBJECTS)))
C_OBJECTS:=$(subst ..,$(UPDIR),$(addprefix $(OBJDIR)/, $(C_OBJECTS)))
S_OBJECTS:=$(subst ..,$(UPDIR),$(addprefix $(OBJDIR)/, $(S_OBJECTS)))

#######################################
# decide which linker to use C or C++
#######################################
ifeq ($(strip $(CXX_SOURCES)),)
	LD=$(CC)
else
	LD=$(CXX)
endif

#######################################
# final list of objects
#######################################
LINK_OBJECTS=$(S_OBJECTS) $(C_OBJECTS) $(CXX_OBJECTS) $(EXT_OBJECTS)

#######################################
# list of desired artefacts
#######################################
EXECUTABLE:=$(BUILDDIR)/$(EXE_NAME)$(EXE_EXT)
BINARY:=$(BUILDDIR)/$(basename $(EXE_NAME)).bin
SHARED_LIB:=$(BUILDDIR)/$(basename lib$(EXE_NAME))$(SHARED_LIB_EXT)
STATIC_LIB:=$(BUILDDIR)/$(basename lib$(EXE_NAME))$(STATIC_LIB_EXT)
LISTING:=$(BUILDDIR)/$(basename $(EXE_NAME)).lst

ifeq ($(strip $(MAKE_BINARY)),yes)
	ARTEFACTS+=$(BINARY)
endif
ifeq ($(strip $(MAKE_EXECUTABLE)),yes)
	ARTEFACTS+=$(EXECUTABLE)
endif

ifeq ($(strip $(MAKE_SHARED_LIB)),yes)
	ARTEFACTS+=$(SHARED_LIB)
	CFLAGS+=-fpic
endif

ifeq ($(strip $(MAKE_STATIC_LIB)),yes)
	ARTEFACTS+=$(STATIC_LIB)
endif

ifeq ($(strip $(CREATE_LST)),yes)
	ARTEFACTS+=$(LISTING)
endif

#######################################
# CLEAN
#######################################
.PHONY: all clean debug-make

all: $(ARTEFACTS)

debug-make:
	@echo "C:        " $(C_SOURCES)
	@echo ""
	@echo "CXX:      " $(CXX_SOURCES)
	@echo ""
	@echo "S:        " $(S_SOURCES)
	@echo ""
	@echo "Co:       " $(C_OBJECTS)
	@echo ""
	@echo "CXXo:     " $(CXX_OBJECTS)
	@echo ""
	@echo "So:       " $(S_OBJECTS)
	@echo ""
	@echo "Lnk Obj:  " $(LINK_OBJECTS)
	@echo ""
	@echo "Artefacts:" $(ARTEFACTS)
	@echo ""
	@echo "Sources:  " $(SOURCES)
	@echo ""
	@echo "INCDIR:   " $(INCDIR)
	@echo ""
	@echo "EXT OBJ:  " $(EXT_OBJECTS)

ifneq ($(FOREIGN_MAKE_TARGETS),)
clean: clean_foreign_targets
endif

clean:
	$(Q)rm -fr $(BUILDDIR)

#######################################
# assembler targets
#######################################
.SECONDEXPANSION:
$(OBJDIR)/%.o: $$(subst $(UPDIR),..,%.s) $(SELFDEP)
	@mkdir -p $(@D)
	$(VECHO) ' [$(CLGRN)S$(CLRST)]   $< ...\n'
	$(Q)$(CC) $(FLAGS) $(CFLAGS) $< -o $@

.SECONDEXPANSION:
$(OBJDIR)/%.o: $$(subst $(UPDIR),..,%.S) $(SELFDEP)
	@mkdir -p $(@D)
	$(VECHO) ' [$(CLGRN)S$(CLRST)]   $< ...\n'
	$(Q)$(CC) $(FLAGS) $(CFLAGS) $< -o $@

#######################################
# C targets
#######################################
.SECONDEXPANSION:
$(OBJDIR)/%.o: $$(subst $(UPDIR),..,%.c) $(SELFDEP)
	@mkdir -p $(@D)
	$(VECHO) ' [$(CLGRN)C$(CLRST)]   $< ...\n'
	$(Q)$(CC) $(FLAGS) $(CFLAGS) $< -o $@

#######################################
# C++ targets
#######################################
.SECONDEXPANSION:
$(OBJDIR)/%.o: $$(subst $(UPDIR),..,%.cpp) $(SELFDEP)
	@mkdir -p $(@D)
	$(VECHO) ' [$(CLYEL)C++$(CLRST)] $< ...\n'
	$(Q)$(CXX) $(FLAGS) $(CXXFLAGS) $< -o $@

.SECONDEXPANSION:
$(OBJDIR)/%.o: $$(subst $(UPDIR),..,%.C) $(SELFDEP)
	@mkdir -p $(@D)
	$(VECHO) ' [$(CLYEL)C++$(CLRST)] $< ...\n'
	$(Q)$(CXX) $(FLAGS) $(CXXFLAGS) $< -o $@

.SECONDEXPANSION:
$(OBJDIR)/%.o: $$(subst $(UPDIR),..,%.cxx) $(SELFDEP)
	@mkdir -p $(@D)
	$(VECHO) ' [$(CLYEL)C++$(CLRST)] $< ...\n'
	$(Q)$(CXX) $(FLAGS) $(CXXFLAGS) $< -o $@

.SECONDEXPANSION:
$(OBJDIR)/%.o: $$(subst $(UPDIR),..,%.cc) $(SELFDEP)
	@mkdir -p $(@D)
	$(VECHO) ' [$(CLYEL)C++$(CLRST)] $< ...\n'
	$(Q)$(CXX) $(FLAGS) $(CXXFLAGS) $< -o $@

.SECONDEXPANSION:
$(OBJDIR)/%.o: $$(subst $(UPDIR),..,%.c++) $(SELFDEP)
	@mkdir -p $(@D)
	$(VECHO) ' [$(CLYEL)C++$(CLRST)] $< ...\n'
	$(Q)$(CXX) $(FLAGS) $(CXXFLAGS) $< -o $@

#######################################
# Build targets
#######################################
$(EXECUTABLE): $(LINK_OBJECTS)
	$(VECHO) ' [$(CLRED)LE$(CLRST)]  $(CLRED)$@$(CLRST) ...\n'
	$(Q)$(LD) -o $@  $(LDFLAGS) $(subst ..,up,$(LINK_OBJECTS)) $(EXT_OBJECTS) $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(EXT_LIBS)) 

$(BINARY): $(EXECUTABLE)
	$(VECHO) ' [$(CLRED)B$(CLRST)]   $(CLRED)$@$(CLRST) ...\n'
	$(Q)$(OC) -O binary $< $@

$(SHARED_LIB): $(LINK_OBJECTS)
	$(VECHO)  ' [$(CLRED)LL$(CLRST)]   $(CLRED)$@$(CLRST) ...\n'
	$(Q)$(LD) -shared -o $@  $(LDFLAGS) $(subst ..,up,$(LINK_OBJECTS)) $(EXT_OBJECTS) $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(EXT_LIBS)) 

$(STATIC_LIB): $(LINK_OBJECTS)
	$(VECHO)  ' [$(CLRED)AR$(CLRST)]  $(CLRED)$@$(CLRST) ...\n'
	$(Q)$(AR) -rc $@  $(subst ..,up,$(LINK_OBJECTS)) $(EXT_OBJECTS) 
	$(VECHO)  ' [$(CLRED)RL$(CLRST)]  $(CLRED)$@$(CLRST) ...\n'
	$(Q)$(RL) $@ 

$(LISTING) : $(EXECUTABLE)
	$(VECHO) ' [$(CLRED)LST$(CLRST)]   $(CLRED)$@$(CLRST) ...\n'
	$(Q)$(LST) -d -t -S $< >$(@)

#######################################
# Include header dependencies
#######################################
-include $(C_OBJECTS:.o=.d)
-include $(CXX_OBJECTS:.o=.d)

#######################################
# Foreign targets dependencies
#######################################
.PHONY: $(FOREIGN_MAKE_TARGETS)
$(FOREIGN_MAKE_TARGETS):
	$(MAKE) -C $(subst build/,,$(dir $@))

.PHONY: clean_foreign_targets
clean_foreign_targets:
	$(foreach var,$(FOREIGN_MAKE_TARGETS),$(MAKE) -C $(subst build/,,$(dir $(var))) clean;)

#######################################
# FLAGS
#######################################
# COMMON_WARN += -Wabi
COMMON_WARN += -Warray-bounds
COMMON_WARN += -Wattributes
COMMON_WARN += -Wcast-align
COMMON_WARN += -Wcast-qual
COMMON_WARN += -Wclobbered
# COMMON_WARN += -Wconversion
COMMON_WARN += -Wcoverage-mismatch
COMMON_WARN += -Wdisabled-optimization
COMMON_WARN += -Wfloat-equal
COMMON_WARN += -Wformat
COMMON_WARN += -Wformat-nonliteral
COMMON_WARN += -Wformat-security
COMMON_WARN += -Wformat-y2k
COMMON_WARN += -Wformat=2
COMMON_WARN += -Wimplicit-fallthrough
COMMON_WARN += -Winit-self
COMMON_WARN += -Winline
COMMON_WARN += -Winvalid-pch
COMMON_WARN += -Wlogical-op
COMMON_WARN += -Wmissing-declarations
COMMON_WARN += -Wmissing-format-attribute
COMMON_WARN += -Wmissing-include-dirs
# COMMON_WARN += -Wmissing-noreturn
COMMON_WARN += -Wno-attributes
COMMON_WARN += -Wno-builtin-macro-redefined
COMMON_WARN += -Wno-deprecated
COMMON_WARN += -Wno-deprecated-declarations
COMMON_WARN += -Wno-div-by-zero
COMMON_WARN += -Wno-endif-labels
COMMON_WARN += -Wno-format-contains-nul
COMMON_WARN += -Wno-format-extra-args
COMMON_WARN += -Wno-int-in-bool-context
COMMON_WARN += -Wno-int-to-pointer-cast
COMMON_WARN += -Wno-mudflap
COMMON_WARN += -Wno-multichar
COMMON_WARN += -Wno-narrowing
COMMON_WARN += -Wno-overflow
COMMON_WARN += -Wno-pragmas
COMMON_WARN += -Wno-unused
COMMON_WARN += -Wno-unused-function
COMMON_WARN += -Wnonnull
COMMON_WARN += -Woverlength-strings
COMMON_WARN += -Wpacked
COMMON_WARN += -Wpacked-bitfield-compat
# COMMON_WARN += -Wpadded
COMMON_WARN += -Wpointer-arith
COMMON_WARN += -Wpointer-sign
COMMON_WARN += -Wredundant-decls
COMMON_WARN += -Wshadow
COMMON_WARN += -Wsign-compare
# COMMON_WARN += -Wsign-conversion
COMMON_WARN += -Wstack-protector
COMMON_WARN += -Wstrict-aliasing=1
COMMON_WARN += -Wstrict-overflow=2
COMMON_WARN += -Wswitch-default
# COMMON_WARN += -Wswitch-enum
COMMON_WARN += -Wsync-nand
COMMON_WARN += -Wsystem-headers
# COMMON_WARN += -Wundef
COMMON_WARN += -Wunknown-pragmas
COMMON_WARN += -Wunreachable-code
COMMON_WARN += -Wunsafe-loop-optimizations
COMMON_WARN += -Wunused-function
COMMON_WARN += -Wvariadic-macros
COMMON_WARN += -Wvla
COMMON_WARN += -Wvolatile-register-var
COMMON_WARN += -Wwrite-strings

C_WARN += -Wimplicit-function-declaration
C_WARN += -Wimplicit-int
C_WARN += -Wincompatible-pointer-types
C_WARN += -Wint-conversion
C_WARN += -Wno-pointer-sign
C_WARN += -Wstrict-prototypes

CXX_WARN += -Wctor-dtor-privacy
CXX_WARN += -Wnon-virtual-dtor
CXX_WARN += -Wreorder
CXX_WARN += -Weffc++
CXX_WARN += -Wold-style-cast
CXX_WARN += -Wstrict-null-sentinel
CXX_WARN += -Wno-non-template-friend
CXX_WARN += -Woverloaded-virtual
CXX_WARN += -Wno-literal-suffix
CXX_WARN += -Wno-pmf-conversions
CXX_WARN += -Wsign-promo
CXX_WARN += -Wno-invalid-offsetof
CXX_WARN += -Wc++0x-compat

C_FULL_FLAGS   += -Wall -Wextra -Wpedantic $(COMMON_WARN) $(C_WARN)
CXX_FULL_FLAGS += -Wall -Wextra -Wpedantic $(COMMON_WARN) $(CXX_WARN)