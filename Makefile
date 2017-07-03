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

package = cpp_tools
version = 0.0.1
tarname = $(package)
distdir = $(tarname)-$(version)

test:
	@make -C test OPT=1 test
.PHONY: test

clean:
	@make -C test clean
	@make -C TinyTest clean
	@rm -rf *.o
.PHONY: clean
