#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

from world.objects.Character import Character
from misc import set_kw

class Villager(Character):
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
        self_kw(self,kw,"nature","human")
    def reveal_operation(self,op):
        # Transform
        pass
