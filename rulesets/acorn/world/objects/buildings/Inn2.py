#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from world.objects.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 6, 8, 2.5
# bmedian = 8, 6, 2.5
# offset = SW corner = 2, -2, 0

class Yetown_inn_deco_agril_2_sw(Thing):
    def setup_operation(self, op):
        ret = Message()
        # South wall
	loc = Location(self, Vector3D(2,-2,0))
        loc.bbox = Vector3D(8,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # West wall with door
	loc = Location(self, Vector3D(2,-2,0))
        loc.bbox = Vector3D(0.5,2,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(2,4,0))
        loc.bbox = Vector3D(0.5,10,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # North wall
	loc = Location(self, Vector3D(2,13.5,0))
        loc.bbox = Vector3D(12,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # East walls
	loc = Location(self, Vector3D(9.5,-2,0))
        loc.bbox = Vector3D(0.5,7,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # South facing segment of east wall
	loc = Location(self, Vector3D(10,2,0))
        loc.bbox = Vector3D(4,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(13.5,2,0))
        loc.bbox = Vector3D(0.5,2,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # Door here
	loc = Location(self, Vector3D(13.5,6,0))
        loc.bbox = Vector3D(0.5,8,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # Internal walls
	loc = Location(self, Vector3D(2,4,0))
        loc.bbox = Vector3D(6,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(7.5,4,0))
        loc.bbox = Vector3D(0.5,7,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        return ret
