#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from cyphesis.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 4,8,2.5
# bmedian = 6,6,2.5
# offset = SW corner = 2,-2,2.5

class House_deco_2(Thing):
    def setup_operation(self, op):
        ret = Oplist()
        # South wall
	loc = Location(self, Vector3D(2, 0, 2))
        loc.bbox = Vector3D(8, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # West wall with door
	loc = Location(self, Vector3D(2, 0, 2))
        loc.bbox = Vector3D(0.5, 5, -2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(2, 0, -4))
        loc.bbox = Vector3D(0.5, 5, -10)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # North wall
	loc = Location(self, Vector3D(2, 0, -13.5))
        loc.bbox = Vector3D(8, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # East wall with door
	loc = Location(self, Vector3D(9.5, 0, 2))
        loc.bbox = Vector3D(0.5, 5, -4)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(9.5, 0, -4))
        loc.bbox = Vector3D(0.5, 5, -10)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # Internal wall
	loc = Location(self, Vector3D(2, 0, -6))
        loc.bbox = Vector3D(4, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        return ret
