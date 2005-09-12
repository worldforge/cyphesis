#!/usr/bin/env python

import sys

if len(sys.argv) != 2:
    print "usage: " + sys.argv[0] + " <opname>"
    sys.exit(1)

opname = sys.argv[1].lower()
opnamecap = opname.capitalize()
opnameup = opname.upper()

outfile = open('common/' + opnamecap + '.h', 'w')

outfile.write("""// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_%s_H
#define COMMON_%s_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int %s_NO;

class %s : public Generic
{
  public:
    %s() {
        (*this)->setType("%s", %s_NO);
    }
};

} } }

#endif // COMMON_%s_H
""" % (opnameup, opnameup, opnameup, opnamecap, opnamecap, opname, opnameup, opnameup))
outfile.close()
