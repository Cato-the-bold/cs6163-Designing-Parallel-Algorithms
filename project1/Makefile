# Put the object filenames here 
# (replace AUTOMATIC_OBJS with list of .o files if you don't want to compile
# all files in this directory into a single executable)
OBJS = $(AUTOMATIC_OBJS) 

# Put the executable name here
TARGET = project1

# Put C preprocessor flags here
CPPFLAGS = 

# C Compiler
CC = mpicc
# Put C compiler flags here (default debugging options, basic optimization)
CFLAGS=-g -O1

# C++ Compiler
CXX = mpiCC
# Put C++ Compiler Flags here (default debugging options, basic optimization)
CXXFLAGS=-g -O1

# Put linker flags here (such as any libraries to link)
LIBRARIES = -lm

#############################################################################
# No need to change rules below this line
#############################################################################

# Find program files in this directory
AUTOMATIC_FILES = $(wildcard *.c *.cc *.C)
AUTOMATIC_OBJS = $(subst .c,.o,$(subst .cc,.o,$(subst .C,.o,$(AUTOMATIC_FILES))))

# Compile target program
$(TARGET): $(OBJS) 
	$(CXX) -o $(TARGET) $(OBJS) $(LIBRARIES)


# rule for generating dependencies from source files
%.d: %.c
	set -e; $(CC) -M $(CPPFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
		[ -s $@ ] || rm -f $@
%.d: %.C
	set -e; $(CXX) -M $(CPPFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
		[ -s $@ ] || rm -f $@
%.d: %.cc
	set -e; $(CXX) -M $(CPPFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
		[ -s $@ ] || rm -f $@

DEPEND_FILES=$(subst .o,.d,$(OBJS))


clean:
	rm -f $(OBJS) $(TARGET) 

distclean:
	rm -f $(OBJS) $(TARGET) $(DEPEND_FILES)

#include automatically generated dependencies
include $(DEPEND_FILES)


