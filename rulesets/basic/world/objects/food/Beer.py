#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Dennis Scott (See the file COPYING for details).

from world.objects.food.Alcohol import Alcohol
from common.misc import set_kw

class Beer(Alcohol):
    """Beer!"""
    def __init__(self, **kw):
    	self.base_init(kw)
    	set_kw(self,kw,"potency",0.1)
