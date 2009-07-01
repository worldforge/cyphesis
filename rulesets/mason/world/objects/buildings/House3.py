#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import Vector3D
from physics import Point3D

import server

# bbox = 4,4,2.5
# bmedian = 3.5,3.5,2.5
# offset = SW corner = -0.5,-0.5

class House3(server.Thing):
    def setup_operation(self, op):
        ret = Oplist()
        # West wall  with door
	loc = Location(self, Point3D(0,0,-1))
        loc.bbox = Vector3D(0.1,2,4)
        ret.append(Operation("create",Entity(parents=['wall'],location=loc),to=self))
	loc = Location(self, Point3D(0,4,-1))
        loc.bbox = Vector3D(0.1,2,4)
        ret.append(Operation("create",Entity(parents=['wall'],location=loc),to=self))
        # South wall
	loc = Location(self, Point3D(0,0,-1))
        loc.bbox = Vector3D(9,0.1,4)
        ret.append(Operation("create",Entity(parents=['wall'],location=loc),to=self))
        # North wall
	loc = Location(self, Point3D(0,5.9,-1))
        loc.bbox = Vector3D(9,0.1,4)
        ret.append(Operation("create",Entity(parents=['wall'],location=loc),to=self))
        # East wall
	loc = Location(self, Point3D(8.9,0,-1))
        loc.bbox = Vector3D(0.1,6,4)
        ret.append(Operation("create",Entity(parents=['wall'],location=loc),to=self))
        return ret
