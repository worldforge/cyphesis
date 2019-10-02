#!/usr/bin/env python

import sys
import compileall

if len(sys.argv) == 1:
    print "usage: " + sys.argv[0] + " <directory< ..."
    sys.exit(1)

for i in range(1, len(sys.argv)):
    print("compiling " + sys.argv[i])
    compileall.compile_dir(sys.argv[i])
