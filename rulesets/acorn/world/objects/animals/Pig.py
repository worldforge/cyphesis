#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
from atlas import *

from world.objects.animals.Animal import Animal
from misc import set_kw
from whrandom import *
from Vector3D import Vector3D

import atlas

class Pig(Animal):
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
        set_kw(self,kw,"weight",5.0)
        self.maxweight=100.0
    def chop_operation(self, op):
        if self.weight<1:
            return(Operation("set",Entity(self.id,status=-1),to=self))
        res = Message()
        ent=Entity(self.id,mode="dead",weight=self.weight-1)
        res.append(Operation("set",ent,to=self))
        ham_ent=Entity(name='ham',type=['ham'])
        if (len(op)>1):
            to_=self.world.get_object(op[1].id)
        else:
            print self.emancipation
            to_=self
        res.append(Operation("create",ham_ent,to=to_))
        return res
