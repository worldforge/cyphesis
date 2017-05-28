#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *

from world.objects.plants.Tree import Tree
from misc import set_kw

"""
This is a special tree which is the actual tree from which apples will
fall. This include special rules for dropping fruits (apples).
"""

class Appletree(Tree):
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
        # The type of fruit #
        set_kw(self,kw,"fruitname","apple")
        set_kw(self,kw,"fruitchance", 15)
        set_kw(self,kw,"init_fruits", 3)
    def touch_operation(self, op):
        # If somebody shakes us we drop a fruit, and make sound #
        fcount = self.fruits
        result = self.drop_fruit()
        if fcount != self.fruits:
            if fcount == 0:
               newmode = "normal"
            else:
               newmode = "fruit"
            result = result + Operation("set",Entity(self.id,fruits=self.fruits,mode=newmode),to=self)
        return result
