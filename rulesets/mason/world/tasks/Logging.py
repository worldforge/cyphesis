#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from Quaternion import Quaternion
from Vector3D import Vector3D

try:
  from random import *
except ImportError:
  from whrandom import *

from world.objects.Thing import Thing

class Logging(Thing):
    """ A proof of concept task for logging."""
    def cut_operation(self, op):
        """ Op handler for cut op which activates this task """
        print "Logging.cut"
        return
    def tick_operation(self, op):
        """ Op handler for regular tick op """
        print "Logging.tick"
        return
