
from atlas import *

from world.objects.plants.Tree import Tree
from misc import set_kw
from whrandom import *


debug_oak = 0

"""
This is a special tree which is the actual tree from which acorns will
fall. This include special rules for dropping fruits (acorns).
"""

class Oak(Tree):
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
