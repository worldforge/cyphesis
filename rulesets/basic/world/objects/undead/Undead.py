#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *
from world.objects.Thing import Thing
from common import log,const
from misc import set_kw

class Undead(Thing):
    """This is the base class for all kinds of undead"""
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
        set_kw(self,kw,"status",1.0)
