#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

from world.objects.Thing import Thing
from misc import set_kw

class Lumber(Thing):
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
        # Burn speed is the proportion of a kg of mass burned per tick
        set_kw(self,kw,"burn_speed",0.01)
        set_kw(self,kw,"material","wood")
        set_kw(self,kw,"mass",10.0)
