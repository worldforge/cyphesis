#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from cyphesis.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 8,8,2.5
# bmedian = 7.5,7.5,2.5
# offset = SW corner = -0.5,-0.5,0

class Farmhouse_deco_1(Thing):
    def setup_operation(self, op):
        ret = Oplist()
        # South wall
	loc = Location(self, Vector3D(-0.5, 0, 0.5))
        loc.bbox = Vector3D(8, 5, -0.2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # West wall with door
	loc = Location(self, Vector3D(-0.5, 0, 0.5))
        loc.bbox = Vector3D(0.2, 5, -2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(-0.5, 0, -3.5))
        loc.bbox = Vector3D(0.2, 5, -12)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # North wall with door
	loc = Location(self, Vector3D(-0.5, 0, -15.3))
        loc.bbox = Vector3D(4, 5, -0.2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(3.5, 0, -15.3))
        loc.bbox = Vector3D(12, 5, -0.2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # East wall
	loc = Location(self, Vector3D(15.3, 0, -7.5))
        loc.bbox = Vector3D(0.2, 5, -8)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # Interior wall
	loc = Location(self, Vector3D(7.3, 0, 0.5))
        loc.bbox = Vector3D(0.2, 5, -14)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # Interior wall with door
	loc = Location(self, Vector3D(7.3, 0, -7.5))
        loc.bbox = Vector3D(4.2, 5, -0.2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(11.5, 0, -7.5))
        loc.bbox = Vector3D(4, 5, -0.2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # South fences
	loc = Location(self, Vector3D(7.5, 0, 0.5))
        loc.bbox = Vector3D(2, 5, -0.1)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(11.5, 0, 0.5))
        loc.bbox = Vector3D(4, 5, -0.1)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # East fences
	loc = Location(self, Vector3D(15.4, 0, 0.5))
        loc.bbox = Vector3D(0.1, 5, -3)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(15.4, 0, -4.5))
        loc.bbox = Vector3D(0.1, 5, -3)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        return ret
