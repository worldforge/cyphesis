#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

from world.objects.Thing import Thing
from world import probability
from common import const

class Alcohol(Thing):
    def move_operation(self, op):
        location=op[0].location.parent
        if hasattr(location,"drunkness") and hasattr(self, "potency"):
            ent=Entity(location.id,drunkness=location.drunkness+self.potency)
            return Operation("set",ent,to=location)
        return Thing.move_operation(self,op)
