#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *

from cyphesis.Thing import Thing
from misc import set_kw

class Campfire(Thing):
    def __init__(self, **kw):
        self.base_init(kw)
        set_kw(self,kw,"mass",5)
    def burn_operation(self, op):
        ret=Oplist()
        for fuel in self.contains:
            if hasattr(fuel,"burn_speed"):
                ret.append(Operation("burn",op[0],to=fuel))
        if op[0].status>1.0:
            to_=op[0].id
            ret.append(Operation("extinguish",op[0],to=to_))
        return ret
