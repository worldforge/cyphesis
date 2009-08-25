#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *
from common import log,const
from random import *

import server

class Weather(server.Thing):
    def tick_operation(self, op):
        world = self.location.parent
        res = Oplist()
        optick = Operation("tick", to=self)
        res = res + optick
        if self.rain<0.1:
            optick.setFutureSeconds(randint(60,300))
            self.rain=uniform(0.1, 0.9)
            self.visibility=10/(self.rain * self.rain)
            res = res + Operation("set", Entity(world.id, moisture=1), to=world)
        else:
            moisture = world.moisture
            optick.setFutureSeconds(randint(600,2400))
            self.rain=0.0
            self.visibility=1000
            res = res + Operation("set", Entity(world.id, moisture=moisture-0.5), to=world)
        res = res+Operation("set", Entity(self.id,rain=self.rain,visibility=self.visibility), to=self)
        return res
