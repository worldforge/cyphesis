#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005 Al Riddoch (See the file COPYING for details).

from atlas import *

from cyphesis.Thing import Thing
from physics import Point3D
from physics import BBox

import server

class Torch(Thing):
    """This is base class for axes, this one just ordinary axe"""
    def ignite_operation(self, op):
        fire_child=None
        for child in self.contains:
            if child.type[0] == 'fire':
                fire_child = child
        if not fire_child:
            return
        to_ = op[0].id
        if not to_:
            return self.error(op,"To is undefined object")
        return Operation("create",Entity(parents=['fire'],status=fire_child.status, location=Location(server.world.get_object(to_),Point3D(0.0,0.0,0.0))),to=to_)
    def burn_operation(self, op):
        fire_status = op[0].status
        to_ = op[0].id
        ret = Message()
        if fire_status < 0.5:
            ret = ret + Operation("nourish", Entity(op[0].id, mass=(0.5 - fire_status)), to=to_)
        fire_loc = Location(self, Point3D(0,0,0.75))
        fire_loc.bbox = BBox(0.05, 0.05, 0.5)
        ret = ret + Operation("move", Entity(to_, location=fire_loc, mode='fixed'), to=to_)
        return ret
