#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

from world.objects.Thing import Thing
from common.misc import set_kw

class Food(Thing):
    def __init__(self, **kw):
        self.base_init(kw)
        set_kw(self,kw,"weight",1.0)
    def eat_operation(self, op):
        ent=Entity(self.id,status=-1)
        res = Operation("set",ent,to=self)
        to_ = self.world.get_object(op[1].id)
        if not to_:
            return self.error(op,"To is undefined object")
        ent.weight=self.weight
        res = res + Operation("nourish",op[1],ent,to=to_,from_=self)
        return res
