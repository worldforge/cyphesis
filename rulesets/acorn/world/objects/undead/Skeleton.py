#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *
from world.objects.Character import Character
from world.objects.undead.Undead import Undead
from mind.SkelMind import SkelMind
from common import log,const
from misc import set_kw
from world.physics.Vector3D import Vector3D
from whrandom import *

class Skeleton(Undead):
    def setup_operation(self, op):
        """do once first after character creation"""
        log.debug(4,"Skeleton.setup_operation:",op)
        if hasattr(op,"sub_to"): return None #meant for mind
        self.mind=SkelMind(id=self.id, body=self)
        opMindSetup=Operation("setup",to=self,sub_to=self.mind)
        return opMindSetup
    def touch_operation(self, op):
        retops = Message()
        if self.status<0: return
        self.status=-1
        newloc=self.location.copy()
        newloc.velocity=Vector3D(0,0,0)
        retops = retops + Operation("move", Entity(self.id, location=newloc.copy()), to=self)
        self.mode="collapsing"
        retops = retops + Operation("set", Entity(self.id, mode=self.mode), to=self)
        retops = retops + Operation("set", Entity(self.id, status=self.status), to=self)
        for item in ['skull', 'ribcage', 'arm', 'pelvis', 'thigh', 'shin']:
            newloc.coordinates = newloc.coordinates + Vector3D(uniform(-1,1), uniform(-1,1), uniform(-1,1))
            retops = retops + Operation("create", Entity(name=item,type=[item],location=newloc.copy()), to=self)
        return retops
