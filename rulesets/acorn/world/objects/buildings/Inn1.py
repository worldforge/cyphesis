#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from world.objects.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 7, 8, 2.5
# bmedian = 6, 9, 2.5
# offset = SW corner = -1, 1, 0

class Yetown_inn_deco_agril_1_sw(Thing):
    def setup_operation(self, op):
        ret = Message()
        # South wall with door
	loc = Location(self, Vector3D(-1,1,0))
        loc.bbox = Vector3D(6,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(9,1,0))
        loc.bbox = Vector3D(4,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # West wall with door
	loc = Location(self, Vector3D(-1,1,0))
        loc.bbox = Vector3D(0.5,2,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(-1,7,0))
        loc.bbox = Vector3D(0.5,4,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # North facing section
	loc = Location(self, Vector3D(-1,10.5,0))
        loc.bbox = Vector3D(6,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # Remaining west facing section
	loc = Location(self, Vector3D(5,7,0))
        loc.bbox = Vector3D(0.5,10,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # North wall
	loc = Location(self, Vector3D(5,16.5,0))
        loc.bbox = Vector3D(8,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # East wall with door
	loc = Location(self, Vector3D(12.5,1,0))
        loc.bbox = Vector3D(0.5,4,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(12.5,7,0))
        loc.bbox = Vector3D(0.5,10,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        return ret
