#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from cyphesis.Thing import Thing
from misc import set_kw
from atlas import *
from Vector3D import Vector3D

# bbox = 5,7,z
# bmedian = 5.5,5.5,z
# offset = SW corner = 0.5, -1.5

class Shop_itemshop_deco_agril_1_sesw(Thing):
    def setup_operation(self, op):
        ret = Oplist()
        # South wall
	loc = Location(self, Vector3D(0.5, 0, 1.5))
        loc.bbox = Vector3D(10, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # West wall with door
	loc = Location(self, Vector3D(0.5, 0, 1.5))
        loc.bbox = Vector3D(0.5, 5, -2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(0.5, 0, -2.5))
        loc.bbox = Vector3D(0.5, 5, -10)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # North wall
	loc = Location(self, Vector3D(0.5, 0, -12))
        loc.bbox = Vector3D(10, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # East walls - FIXME There is a door in this first wall
	loc = Location(self, Vector3D(10, 0, 1.5))
        loc.bbox = Vector3D(0.5, 5, -8)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(7.5, 0, -6))
        loc.bbox = Vector3D(3, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(8, 0, -6.5))
        loc.bbox = Vector3D(0.5, 5, -6)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # Pillar
	loc = Location(self, Vector3D(10, 0, -9.5))
        loc.bbox = Vector3D(0.5, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        return ret
