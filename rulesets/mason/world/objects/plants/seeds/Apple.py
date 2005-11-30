#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *

from world.objects.Thing import Thing
from world.objects.plants.seeds.Seed import Seed

import atlas


class Apple(Seed):
    def eat_operation(self, op):
        ent=Entity(self.id,status=-1)
        res = Operation("set",ent,to=self)
        to_ = op.from_
        nour=Entity(to_,mass=self.mass,alcohol=self.alcohol)
        res = res + Operation("nourish",nour,to=to_)
        return res
    def setup_operation(self, op):
        # Decay a bit, we don't last forever
        opTick=Operation("tick",to=self)
        opTick.setFutureSeconds(900)
        return opTick
    def tick_operation(self, op):
        # After a short while we turn into a rotten apple, which is alcoholic
        if self.alcohol > 0:
            ent=Entity(self.id,status=-1)
            return Operation("set",ent,to=self)
        else:
            self.alcohol = 1
            opTick=Operation("tick",to=self)
            opTick.setFutureSeconds(900)
            return opTick
