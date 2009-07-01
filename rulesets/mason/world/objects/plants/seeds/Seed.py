from atlas import *

from common import log,const
try:
  from random import *
except ImportError:
  from whrandom import *

import atlas
import server

debug_seed = 0

speed = 1

class Seed(server.Thing):
    """Base for kinds of seeds and fruit."""
    def tick_operation(self, op): pass
#        opTick=Operation("tick",to=self)
#        opTick.setFutureSeconds(const.basic_tick*speed)
#
#        result = atlas.Oplist(opTick)
#
#        new_status=self.status-0.1
#        ent=Entity(self.id,status=new_status)
#        result = result + Operation("set",ent,to=self)
#        return result
