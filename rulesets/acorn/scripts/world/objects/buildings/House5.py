#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from cyphesis.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 6,6,2.5
# bmedian = 5.5,5.5,2.5
# offset = SW corner = -0.5,-0.5,2.5

class House_deco_5(Thing):
    def setup_operation(self, op):
        ret = Oplist()
        # Outer area
        # South wall
	loc = Location(self, Vector3D(-0.5, 0, 0.5))
        loc.bbox = Vector3D(4, 1, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # Left of entrance walls
	loc = Location(self, Vector3D(-0.5, 0, -3.5))
        loc.bbox = Vector3D(2, 1, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(-0.5, 0, -3.5))
        loc.bbox = Vector3D(0.5, 1, -4)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # Full height walls
	loc = Location(self, Vector3D(-0.5, 0, -7.5))
        loc.bbox = Vector3D(0.5, 5, -4)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # Main structure
        # South wall
	loc = Location(self, Vector3D(3.5, 0, 0.5))
        loc.bbox = Vector3D(8, 1, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # West wall with door
	loc = Location(self, Vector3D(3.5, 0, 0.5))
        loc.bbox = Vector3D(0.5, 1, -2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(3.5, 0, -3.5))
        loc.bbox = Vector3D(0.5, 1, -8)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # North wall
	loc = Location(self, Vector3D(-0.5, 0, -11))
        loc.bbox = Vector3D(12, 1, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # East wall with door
	loc = Location(self, Vector3D(11, 0, 0.5))
        loc.bbox = Vector3D(0.5, 1, -2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(11, 0, -3.5))
        loc.bbox = Vector3D(0.5, 1, -8)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # Interior wall
	loc = Location(self, Vector3D(7.5, 0, -4.5))
        loc.bbox = Vector3D(4, 1, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        return ret
