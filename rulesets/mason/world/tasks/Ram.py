#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from Quaternion import Quaternion
from Vector3D import Vector3D

try:
  from random import *
except ImportError:
  from whrandom import *

from world.objects.Thing import Thing

class Ram(Thing):
    """ A proof of concept task for drivinf object into the ground."""
    def strike_operation(self, op):
        """ Op handler for strike op which activates this task """
        if len(op) < 1:
            print "No target"

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to
    def tick_operation(self, op):
        """ Op handler for regular tick op """
