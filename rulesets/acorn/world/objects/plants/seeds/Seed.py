from atlas import *

from world.objects.Thing import Thing
from misc import set_kw
from common import log,const
from whrandom import *

import atlas

debug_seed = 0

speed = 1

"""
Base class from which all fruit/seed entities are derived.
"""

class Seed(Thing):
    def __init__(self, **kw):
        self.base_init(kw)
        set_kw(self,kw,"weight",0.005)
    def tick_operation(self, op): pass
#        opTick=Operation("tick",to=self)
#        opTick.time.sadd=const.basic_tick*speed
#
#        result = atlas.Message(opTick)
#
#        new_status=self.status-0.1
#        ent=Entity(self.id,status=new_status)
#        result = result + Operation("set",ent,to=self)
#        return result
