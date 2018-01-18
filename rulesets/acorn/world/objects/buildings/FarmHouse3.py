#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from cyphesis.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 5,7,2.5
# bmedian = 6,6,2.5
# offset = SW corner = 1,-1,0

class Farmhouse_deco_3(Thing):
    def setup_operation(self, op):
        ret = Oplist()
        # South wall
	loc = Location(self, Vector3D(1, 0, 1))
        loc.bbox = Vector3D(10, 5, -0.2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # West wall
	loc = Location(self, Vector3D(1, 0, 1))
        loc.bbox = Vector3D(0.2, 5, -6)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(1, 0, -7))
        loc.bbox = Vector3D(0.2, 5, -6)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # North wall with door
	loc = Location(self, Vector3D(1, 0, -12.8))
        loc.bbox = Vector3D(4, 5, -0.2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(7, 0, -12.8))
        loc.bbox = Vector3D(4, 5, -0.2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # East wall
	loc = Location(self, Vector3D(10.8, 0, 1))
        loc.bbox = Vector3D(0.2, 5, -14)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        return ret
