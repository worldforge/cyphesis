#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Kosh (See the file COPYING for details).

from atlas import *

from world.object.Thing import Thing
from common import const
from world import probability
from common.misc import set_kw
from world.object.buildings.House import House

class StoneHouse(House):
    """This class is a stone house it does not burn"""
    def __init__(self, **kw):
    	self.base_init(kw)
    	set_kw(self,kw,"burn_speed",0.0)
        set_kw(self,kw,"material","stone")
