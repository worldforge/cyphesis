#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

from world.objects.Thing import Thing
from misc import set_kw

class Wolf(Thing):
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
        set_kw(self,kw,"weight",20.0)
        self.maxweight=30.0
