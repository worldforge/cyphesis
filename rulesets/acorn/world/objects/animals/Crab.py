#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from world.objects.animals.Animal import Animal
from common.misc import set_kw

class Crab(Animal):
    def __init__(self, **kw):
        self.base_init(kw)
        set_kw(self,kw,"weight",20.0)
        self.maxweight=30.0
