###############################################################################
#
# Generic Makefile for C/C++ Program
#
# Author: whyglinux (whyglinux AT hotmail DOT com)
# Date: 2006/03/04

# Description:
# The makefile searches in directories for the source files
# with extensions specified in , then compiles the sources
# and finally produces the , the executable file, by linking
# the objectives.

# Usage:
# $ make compile and link the program.
# $ make objs compile only (no linking. Rarely used).
# $ make clean clean the objectives and dependencies.
# $ make cleanall clean the objectives, dependencies and executable.
# $ make rebuild rebuild the program. The same as make clean && make all.
#==============================================================================

## Customizing Section: adjust the following if necessary.
##=============================================================================

############### Added by cxp ##############
TARGET_NAME := my_tools.exe
PREFIX ?= $(CROSS_COMPILE)
POSTFIX := $(CROSS_COMPILE_POSTFIX)
AS := $(PREFIX)as$(POSTFIX)
LD := $(PREFIX)gcc$(POSTFIX)
###########################################

# The executable file name.
# It must be specified.
PROGRAM := $(TARGET_NAME)

# The directories in which source files reside.
# At least one path should be specified.
# SRCDIRS := . # current directory
SRCDIRS := . common lua lualib ximage resource_manager messagepeer  
SRCDIRS += process_manager websocket

# The source file types (headers excluded).
# At least one type should be specified.
# The valid suffixes are among of .c, .C, .cc, .cpp, .CPP, .c++, .cp, or .cxx.
# SRCEXTS := .c # C program
# SRCEXTS := .cpp # C++ program
# SRCEXTS := .c .cpp # C/C++ program
SRCEXTS := .cpp .c

# The compiling flags used only for C.
# If it is a C++ program, no need to set these flags.
# If it is a C and C++ merging program, set these flags for the C parts.
CFLAGS := -O2 -Wall -Iplatform -I. -Icommon -Ilua -Ilualib 
CFLAGS += -Iximage -Iresource_manager -Imessagepeer -Iprocess_manager -Iwebsocket
CFLAGS += $(APPEND_CFLAGS)

# The compiling flags used only for C++.
# If it is a C program, no need to set these flags.
# If it is a C and C++ merging program, set these flags for the C++ parts.
CXXFLAGS := $(CFLAGS)

OBJCOPY := $(PREFIX)objcopy
OBJCOPY +=

# The library and the link options ( C and C++ common).
LDFLAGS := $(APPEND_LDFLAGS) -lpthread -lstdc++ -lm -ldl
## Implict Section: change the following only when necessary.
##=============================================================================
# The C program compiler. Uncomment it to specify yours explicitly.
CC = $(PREFIX)gcc$(POSTFIX)

# The C++ program compiler. Uncomment it to specify yours explicitly.
CXX = $(PREFIX)g++$(POSTFIX)

# Uncomment the 2 lines to compile C programs as C++ ones.
#CC = $(CXX)
#CFLAGS = $(CXXFLAGS)

# The command used to delete file.
RM = rm -f

## Stable Section: usually no need to be changed. But you can add more.
##=============================================================================
SHELL = /bin/sh
SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))

OBJS = $(foreach x,$(SRCEXTS), \
$(patsubst %$(x),%.o,$(filter %$(x),$(SOURCES))))

DEPS = $(patsubst %.o,%.d,$(OBJS))

.PHONY : all objs clean cleanall rebuild

all : $(PROGRAM)

# Rules for creating the dependency files (.d).
#---------------------------------------------------
%.d : %.c
	$(CC) -MM -MD $(CFLAGS) $< -o $@

%.d : %.C
	$(CC) -MM -MD $(CXXFLAGS) $< -o $@

%.d : %.cc
	$(CC) -MM -MD $(CXXFLAGS) $< -o $@

%.d : %.cpp
	$(CXX) -MM -MD $(CXXFLAGS) $< -o $@

%.d : %.CPP
	$(CXX) -MM -MD $(CXXFLAGS) $< -o $@

%.d : %.c++
	$(CXX) -MM -MD $(CXXFLAGS) $< -o $@

%.d : %.cp
	$(CXX) -MM -MD $(CXXFLAGS) $< -o $@

%.d : %.cxx
	$(CXX) -MM -MD $(CXXFLAGS) $< -o $@

# Rules for producing the objects.
#---------------------------------------------------
objs : $(OBJS)
%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o : %.C
	$(CC) -c $(CXXFLAGS) $< -o $@

%.o : %.cc
	$(CC) -c $(CXXFLAGS) $< -o $@

%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.o : %.CPP
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.o : %.c++
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.o : %.cp
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.o : %.cxx
	$(CXX) -c $(CXXFLAGS) $< -o $@

# Rules for producing the executable.
#----------------------------------------------
$(PROGRAM) : $(OBJS)
ifeq ($(strip $(SRCEXTS)), .c) # C file
	$(LD) -o $(PROGRAM) $(OBJS) $(LDFLAGS)
else # C++ file
	$(LD) -o $(PROGRAM) $(OBJS) $(LDFLAGS)
endif

-include $(DEPS)

rebuild: clean all

clean :
	for name in `echo $(SRCDIRS)`; \
    do \
        $(RM) $$name/*.o $$name/*.d $(PROGRAM); \
    done

#############################################################################
### End of the Makefile ## Suggestions are welcome ## All rights reserved ###
#############################################################################
