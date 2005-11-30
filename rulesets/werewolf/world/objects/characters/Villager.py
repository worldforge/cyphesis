#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

from world.objects.Character import Character

class Villager(Character):
    def reveal_operation(self,op):
        # Transform
        pass
    def bless_operation(self,op):
        self.nature = "seer"
        i = Operation("imaginary", Entity(description="is a seer"))
        return Operation("sight", i, from_=self, to=self)
    def curse_operation(self,op):
        self.nature = "werewolf"
        i = Operation("imaginary", Entity(description="is a werewolf"))
        return Operation("sight", i, from_=self, to=self)
    def cleanse_operation(self,op):
        self.nature = "human"
        i = Operation("imaginary", Entity(description="is a human"))
        return Operation("sight", i, from_=self, to=self)
