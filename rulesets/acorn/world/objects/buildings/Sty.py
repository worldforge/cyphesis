#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from world.objects.Thing import Thing
from common.misc import set_kw

class Sty(Thing):
    def __init__(self, **kw):
    	self.base_init(kw)
        set_kw(self,kw,"width",4.0)
        set_kw(self,kw,"length",4.0)
