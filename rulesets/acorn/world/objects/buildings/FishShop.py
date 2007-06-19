#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from cyphesis.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 5,7,2.5
# bmedian = 5.5,6.5,2.5
# offset = SW corner = 0.5,-0.5

class Shop_fishshop_deco_agril_1_sesw(Thing):
    def setup_operation(self, op):
        ret = Message()
        # South wall with door
	loc = Location(self, Vector3D(0.5,-0.5,0))
        loc.bbox = Vector3D(2,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(3.5,-0.5,0))
        loc.bbox = Vector3D(6,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # West wall with door
	loc = Location(self, Vector3D(0.5,-0.5,0))
        loc.bbox = Vector3D(0.5,10,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(0.5,11.5,0))
        loc.bbox = Vector3D(0.5,2,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # North wall
	loc = Location(self, Vector3D(0.5,13,0))
        loc.bbox = Vector3D(8,0.5,5)
        # East wall sections with door
	loc = Location(self, Vector3D(10,-0.5,0))
        loc.bbox = Vector3D(0.5,2,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(10,3.5,0))
        loc.bbox = Vector3D(0.5,4,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # North facing section
	loc = Location(self, Vector3D(6,7,0))
        loc.bbox = Vector3D(4.5,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
	loc = Location(self, Vector3D(6,7.5,0))
        loc.bbox = Vector3D(0.5,6,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        return ret
