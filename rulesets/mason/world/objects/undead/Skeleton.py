#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *
from world.objects.Thing import Thing
from common import log,const
from misc import set_kw
from Vector3D import Vector3D
from whrandom import *

class Skeleton(Thing):
    def touch_operation(self, op):
        retops = Message()
        if self.status<0: return
        self.status=-1
        newloc=self.location.copy()
        newloc.velocity=Vector3D(0,0,0)
        retops = retops + Operation("move", Entity(self.id, location=newloc.copy()), to=self)
        self.mode="collapsed"
        retops = retops + Operation("action", Entity(self.id, action="collapse"), to=self)
        for item in ['skull', 'ribcage', 'arm', 'pelvis', 'thigh', 'shin']:
            newloc.coordinates = newloc.coordinates + Vector3D(uniform(-1,1), uniform(-1,1), uniform(-1,1))
            retops = retops + Operation("create", Entity(name=item,parents=[item],location=newloc.copy()), to=self)
        retops = retops + Operation("set", Entity(self.id, mode=self.mode, status=self.status), to=self)
        return retops
