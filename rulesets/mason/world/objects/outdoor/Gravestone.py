#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005 Erik Hjortsberg (See the file COPYING for details).

from atlas import *
from world.objects.Thing import Thing
from common import log,const
from misc import set_kw
from Vector3D import Vector3D
try:
  from random import *
except ImportError:
  from whrandom import *


"""
When digging at a grave, skeleton parts will be created. We might want to add some waiting period or some kind of capacity though.

/erik
"""

def create_skeletonpart(self, op):
    retops = Message()
    newloc=self.location.copy()
    newloc.velocity=Vector3D(0,0,0)
    items = ['skull', 'ribcage', 'arm', 'pelvis', 'thigh', 'shin']
    item = items[randint(0,5)]
    newloc.coordinates = newloc.coordinates + Vector3D(uniform(-1,1), uniform(-1,1), uniform(-1,1))
    retops = retops + Operation("create", Entity(name=item,parents=[item],location=newloc.copy()), to=self)
    return retops

class Gravestone_stylish(Thing):
    def dig_operation(self, op):
        return create_skeletonpart(self, op)    

class Gravestone_rounded(Thing):
    def dig_operation(self, op):
        return create_skeletonpart(self, op)    
     
class Gravestone_simple(Thing):
    def dig_operation(self, op):
        return create_skeletonpart(self, op)    
     
