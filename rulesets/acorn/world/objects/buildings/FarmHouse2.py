#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from world.objects.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 7,5,2.5
# bmedian = 3,2,2.5
# offset = SW corner = -4,-3,0

class Farmhouse_deco_3(Thing):
    def setup_operation(self, op):
        ret = Message()
        # South wall
	loc = Location(self, Vector3D(-4,3,0))
        loc.bbox = Vector3D(7,0.1,2.5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # West wall
	loc = Location(self, Vector3D(-4,3,0))
        loc.bbox = Vector3D(0.1,2,2.5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # North wall
	loc = Location(self, Vector3D(-4,6.8,0))
        loc.bbox = Vector3D(7,0.1,2.5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # East wall
	loc = Location(self, Vector3D(9.8,-3,0))
        loc.bbox = Vector3D(0.1,5,2.5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # West annex wall
	loc = Location(self, Vector3D(4,-3,0))
        loc.bbox = Vector3D(0.1,3,2.5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # South annex wall
	loc = Location(self, Vector3D(4,-3,0))
        loc.bbox = Vector3D(3,0.1,2.5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        return ret
