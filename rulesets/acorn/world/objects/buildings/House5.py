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
        ret = Message()
        # Outer area
        # South wall
	loc = Location(self, Vector3D(-0.5,-0.5,0))
        loc.bbox = Vector3D(4,0.5,1)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # Left of entrance walls
	loc = Location(self, Vector3D(-0.5,3.5,0))
        loc.bbox = Vector3D(2,0.5,1)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(-0.5,3.5,0))
        loc.bbox = Vector3D(0.5,4,1)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # Full height walls
	loc = Location(self, Vector3D(-0.5,7.5,0))
        loc.bbox = Vector3D(0.5,4,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # Main structure
        # South wall
	loc = Location(self, Vector3D(3.5,-0.5,0))
        loc.bbox = Vector3D(8,0.5,1)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # West wall with door
	loc = Location(self, Vector3D(3.5,-0.5,0))
        loc.bbox = Vector3D(0.5,2,1)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(3.5,3.5,0))
        loc.bbox = Vector3D(0.5,8,1)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # North wall
	loc = Location(self, Vector3D(-0.5,11,0))
        loc.bbox = Vector3D(12,0.5,1)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # East wall with door
	loc = Location(self, Vector3D(11,-0.5,0))
        loc.bbox = Vector3D(0.5,2,1)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(11,3.5,0))
        loc.bbox = Vector3D(0.5,8,1)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # Interior wall
	loc = Location(self, Vector3D(7.5,4.5,0))
        loc.bbox = Vector3D(4,0.5,1)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        return ret
