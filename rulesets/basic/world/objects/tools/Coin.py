#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

from world.objects.Thing import Thing
from misc import set_kw

class Coin(Thing):
    def __init__(self, **kw):
        self.base_init(kw)
        set_kw(self,kw,"weight",0.01)
        set_kw(self,kw,"material","gold")
