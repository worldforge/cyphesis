#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from cyphesis.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 4,8,2.5
# bmedian = 6,6,2.5
# offset = SW corner = 2, -2, 0

class Mansion_grey_deco_1_sw(Thing):
    def setup_operation(self, op):
        ret = Oplist()
        # South wall with door
	loc = Location(self, Vector3D(2,-2,0))
        loc.bbox = Vector3D(2,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(8,-2,0))
        loc.bbox = Vector3D(2,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # West wall
	loc = Location(self, Vector3D(2,-2,0))
        loc.bbox = Vector3D(0.5,16,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # East wall
	loc = Location(self, Vector3D(10,-2,0))
        loc.bbox = Vector3D(0.5,16,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # North wall
	loc = Location(self, Vector3D(2,13.5,0))
        loc.bbox = Vector3D(8,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # Internal wall
	loc = Location(self, Vector3D(2,6,0))
        loc.bbox = Vector3D(4,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # Internal short wall by stairs
	loc = Location(self, Vector3D(8,6,0))
        loc.bbox = Vector3D(2,0.5,0.5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        return ret
