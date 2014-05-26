#    Copyright (C) 2014 University of Southern California and
#                       Philip J. Uren
#
#    Authors: Philip J. Uren
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

# what unit tests to build
TESTS=testIntervalTree

# where is TinyTest, the smithlab common library and the common code for 
# this package?
TINY_TEST=../TinyTest
COMMON=..

# check for darwin; adjust architecture and stl usage.
ifeq "$(shell uname)" "Darwin"
CPPFLAGS += -arch x86_64
ifeq "$(shell if [ `sysctl -n kern.osrelease | cut -d . -f 1` -ge 13 ];\
              then echo 'true'; fi)" "true"
CPPFLAGS += -stdlib=libstdc++
endif
endif

# set up compiler flags
CXX = g++
CFLAGS = -g -Wall -fPIC -fmessage-length=50
OPTFLAGS = -O3
DEBUGFLAGS =

ifdef DEBUG
CPPFLAGS += $(DEBUGFLAGS)
endif

ifdef OPT
CPPFLAGS += $(OPTFLAGS)
endif

LIBS = -lgsl -lgslcblas 
INCLUDE_ARGS=-I$(TINY_TEST) -I$(COMMON)

# test dependencies
$(TESTS): $(addprefix $(TINY_TEST)/, TinyTest.o) 
#$(TESTS): $(addprefix $(COMMON)/, IntervalTree.o)

test% : test%.cpp
	$(CXX) $(CPPFLAGS) $(CFLAGS) $^ $(INCLUDE_ARGS) $(LIBS) -o $@
	
%.o: %.cpp %.hpp
	$(CXX) $(CPPFLAGS) -c -o $@ $< $(INCLUDE_ARGS)	

test : $(TESTS)
	for f in $^; do ./$$f; done
.PHONY : test

all : test
.PHONY : all

clean :
	rm -rf *.o *.dSYM $(TESTS)
	 