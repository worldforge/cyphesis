#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *
from cyphesis.Thing import Thing
from common import log,const
try:
  from random import *
except ImportError:
  from whrandom import *

class Weather(Thing):
    def tick_operation(self, op):
        res = Oplist()
        optick = Operation("tick", to=self)
        res = res + optick
        if self.rain<0.1:
            optick.setFutureSeconds(randint(60,300))
            self.rain=uniform(0.1, 0.9)
            self.visibility=10/(self.rain * self.rain)
        else:
            optick.setFutureSeconds(randint(600,2400))
            self.rain=0.0
            self.visibility=1000
        res = res+Operation("set", Entity(self.id,rain=self.rain,visibility=self.visibility), to=self)
        return res
