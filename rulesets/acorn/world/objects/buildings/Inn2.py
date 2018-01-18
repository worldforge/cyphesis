#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from cyphesis.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 6, 8, 2.5
# bmedian = 8, 6, 2.5
# offset = SW corner = 2, -2, 0

class Yetown_inn_deco_agril_2_sw(Thing):
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
        loc.bbox = Vector3D(12, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # East walls
	loc = Location(self, Vector3D(9.5, 0, 2))
        loc.bbox = Vector3D(0.5, 5, -7)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # South facing segment of east wall
	loc = Location(self, Vector3D(10, 0, -2))
        loc.bbox = Vector3D(4, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(13.5, 0, -2))
        loc.bbox = Vector3D(0.5, 5, -2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # Door here
	loc = Location(self, Vector3D(13.5, 0, -6))
        loc.bbox = Vector3D(0.5, 5, -8)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # Internal walls
	loc = Location(self, Vector3D(2, 0, -4))
        loc.bbox = Vector3D(6, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(7.5, 0, -4))
        loc.bbox = Vector3D(0.5, 5, -7)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        return ret
