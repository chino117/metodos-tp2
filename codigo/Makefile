# Tests
CC=g++ # This is the main compiler

SRCDIR=src
BUILDDIR=build
TESTDIR=tests

SRCEXT=cpp
SOURCES=$(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS= -std=c++11 -Wall -Wfloat-equal -O3
LIB=
INC=-I include

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Tests
#tester: build/matrix.o build/cholesky.o
#	$(CC) $(CFLAGS) $(TESTS) $^ $(INC) $(LIB) -o bin/tester

all:
	echo $(SOURCES)
clean:
	echo " Cleaning...";
	echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)
	rm -f tp2

tp2: $(OBJECTS)
	$(CC) $(CFLAGS) $(INC) $^ -o tp2