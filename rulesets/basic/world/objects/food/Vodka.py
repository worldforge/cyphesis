#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from world.objects.food.Alcohol import Alcohol
from common.misc import set_kw

class Vodka(Alcohol):
    """More potent than beer"""
    def __init__(self, cppthing, **kw):
    	self.base_init(cppthing, kw)
    	set_kw(self,kw,"potency",0.2)
