# $Id: Makefile,v 1.2 2002/11/06 14:34:25 dflater Exp $

# This software was developed at the National Institute of Standards
# and Technology by employees of the Federal Government in the course
# of their official duties.  Pursuant to title 17 Section 105 of the
# United States Code this software is not subject to copyright
# protection and is in the public domain.
# 
# You can redistribute it and/or modify it freely provided that any
# derivative works bear some notice that they are derived from it,
# and any modified versions bear some notice that they have been
# modified.  We would appreciate acknowledgement if the software is
# used.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  NIST does not
# assume legal liability or responsibility for anything done with
# this software.

OBJS = Dstr.o sumo2loom.o
HDRS = Dstr.hh header.hh

# -O2 first has -s
sumo2loom: $(OBJS)
	g++ -Wall -std=c++11 -g  -o sumo2loom $(OBJS)

.cc.o:
	g++ -Wall -std=c++11 -g -c $<

$(OBJS): $(HDRS)

clean:
	rm -f *.o sumo2loom
