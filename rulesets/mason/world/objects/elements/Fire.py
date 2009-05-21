#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

from cyphesis.Thing import Thing
from common import const

import server

class Fire(Thing):
    """fire to burn things up"""
    #CHEAT! make it more realistic (like spreading to things that burn near)
    def extinguish_operation(self, op):
        """If somebody tries to extinguish us, change status lower"""
        self.status=self.status-0.25
        self_ent=Entity(self.id,status=self.status)
        #print "Extinguish:",self,self.location.parent,self.status,op.from_
        return Operation("set",self_ent,to=self)
    def tick_operation(self, op):
        """things to do every tick, see comments"""
        #print `self`,"Got tick operation:\n",op
        #Is fire extinguished?
        # if self.status<0.0:
            # return Operation("delete",Entity(self.id),to=self)

        #Have we burned up our parent container?
        if server.world==self.location.parent:
            return Operation("delete",Entity(self.id),to=self)

        #Send burn operation to parent container
        self_ent=Entity(self.id,status=self.status)
        opBurn=Operation("burn",self_ent,to=self.location.parent)
        if self.status<0.2:
            status=self.status-0.02
        else:
            status=self.status-0.02
        if status > 1:
            status=1
        self_ent2=Entity(self.id,status=status)
        opSet=Operation("set",self_ent2,to=self)

        #and call this method later
        opTick=Operation("tick",to=self)
        opTick.setFutureSeconds(const.basic_tick*4)
        return Oplist(opBurn,Operation("sight",opBurn),opTick,opSet)
    def nourish_operation(self, op):
        #Increase fire
        inc=op[0].mass
        self.status=self.status+inc
        #No need to send a set. The next tick will deal with that.
    #def move_operation(self, op): pass
