from atlas import *

from world.objects.Thing import Thing
from world.objects.plants.seeds.Seed import Seed
from common.misc import set_kw

import atlas


class Acorn(Seed):
    def eat_operation(self, op):
        ent=Entity(self.id,status=-1)
        res = Operation("set",ent,to=self)
        to_ = self.world.get_object(op[1].id)
        if not to_:
            return self.error(op,"To is undefined object")
        ent.weight=0.1
        res = res + Operation("nourish",op[1],ent,to=to_,from_=self)
        return res
    def setup_operation(self, op):
        # Decay a bit, we don't last forever
        # Of course, a real acorn would germinate after a bit
        opTick=Operation("tick",to=self)
        opTick.time.sadd=1800
        return opTick
    def tick_operation(self, op):
        ent=Entity(self.id,status=-1)
        return Operation("set",ent,to=self)
