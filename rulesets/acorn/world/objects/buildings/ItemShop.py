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
        ret = Message()
        # South wall
	loc = Location(self, Vector3D(0.5,-1.5,0))
        loc.bbox = Vector3D(10,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # West wall with door
	loc = Location(self, Vector3D(0.5,-1.5,0))
        loc.bbox = Vector3D(0.5,2,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(0.5,2.5,0))
        loc.bbox = Vector3D(0.5,10,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # North wall
	loc = Location(self, Vector3D(0.5,12,0))
        loc.bbox = Vector3D(10,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # East walls - FIXME There is a door in this first wall
	loc = Location(self, Vector3D(10,-1.5,0))
        loc.bbox = Vector3D(0.5,8,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(7.5,6,0))
        loc.bbox = Vector3D(3,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(8,6.5,0))
        loc.bbox = Vector3D(0.5,6,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # Pillar
	loc = Location(self, Vector3D(10,9.5,0))
        loc.bbox = Vector3D(0.5,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        return ret
