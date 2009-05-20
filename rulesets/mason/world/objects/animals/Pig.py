#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
from atlas import *

from cyphesis.Thing import Thing
try:
  from random import *
except ImportError:
  from whrandom import *
from physics import Vector3D

import atlas

class Pig(Thing):
    """ A pig """
    def chop_operation(self, op):
        if self.mass<1:
            return(Operation("set",Entity(self.id,status=-1),to=self))
        res = Message()
        ent=Entity(self.id,mode="dead",mass=self.mass-1)
        res.append(Operation("set",ent,to=self))
        ham_ent=Entity(name='ham',parents=['ham'],location=self.location.copy())
        if (len(op)>1):
            to_ = op[1].id
        else:
            to_=self
        res.append(Operation("create",ham_ent,to=to_))
        return res
