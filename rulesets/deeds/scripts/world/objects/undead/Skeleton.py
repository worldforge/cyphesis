#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *
from common import log,const
from physics import Vector3D
from random import *

import server

class Skeleton(server.Thing):
    def touch_operation(self, op):
        retops = Oplist()
        if self.props.status and self.props.status<0: return
        newloc=self.location.copy()
        newloc.velocity=Vector3D()
        #retops += Operation("move", Entity(self.id, location=newloc.copy(), mode="collapsed"), to=self)
        for item in ['skull', 'ribcage', 'femur', 'pelvis', 'tibia']:
            newloc.position = newloc.position + Vector3D(uniform(-1,1), uniform(-1,1), uniform(-1,1))
            retops += Operation("create", Entity(name=item,parent=item,location=newloc.copy()), to=self)
        retops += Operation("set", Entity(self.id, status=-1), to=self)
        return (server.OPERATION_BLOCKED, retops)
