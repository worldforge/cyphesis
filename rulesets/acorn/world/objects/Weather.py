#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *
from cyphesis.Thing import Thing
from common import log,const
from misc import set_kw
from whrandom import *

class Weather(Thing):
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
        set_kw(self,kw,"rain",1.0)
        set_kw(self,kw,"snow",0.0)
    def tick_operation(self, op):
        res = Oplist()
        optick = Operation("tick", to=self)
        res = res + optick
        if self.rain<0.5:
            optick.setFutureSeconds(randint(120,300))
            self.rain=1.0
        else:
            optick.setFutureSeconds(randint(600,1200))
            self.rain=0.0
        res = res+Operation("set", Entity(self.id,rain=self.rain), to=self)
        return res
