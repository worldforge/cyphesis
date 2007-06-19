#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from cyphesis.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 5,5,2.5
# bmedian = 4.5,4.5,2.5
# offset = SW corner = -0.5,-0.5

class Shop_metalshop_deco_agril_1_sesw(Thing):
    def setup_operation(self, op):
        ret = Message()
        # South wall with door
	loc = Location(self, Vector3D(1.5,-0.5,0))
        loc.bbox = Vector3D(4,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(7.5,-0.5,0))
        loc.bbox = Vector3D(2,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # West wall
	loc = Location(self, Vector3D(-0.5,-0.5,0))
        loc.bbox = Vector3D(0.5,10,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # North wall with door
	loc = Location(self, Vector3D(-0.5,9,0))
        loc.bbox = Vector3D(2,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(3.5,9,0))
        loc.bbox = Vector3D(6,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # East wall
	loc = Location(self, Vector3D(9,-0.5,0))
        loc.bbox = Vector3D(0.5,10,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # Internal walls
	loc = Location(self, Vector3D(3.5,-0.5,0))
        loc.bbox = Vector3D(0.5,4.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(-0.5,3.5,0))
        loc.bbox = Vector3D(4.5,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        return ret
