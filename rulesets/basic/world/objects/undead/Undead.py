#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *
from world.objects.Character import Character
from common import log,const
from common.misc import set_kw

class Undead(Character):
    """This is the base class for all kinds of undead"""
    def __init__(self, **kw):
        self.base_init(kw)
        set_kw(self,kw,"status",1.0)
