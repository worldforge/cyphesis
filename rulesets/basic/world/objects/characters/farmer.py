#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

from world.objects.Thing import *

class farmer(Thing):
    def __init__(self, cppthing, **kw):
        print("farmer.__init__")
        self.base_init(cppthing, kw)
    def sight_operation(self, op):
        print "This is in the python method"
        print self.mass
