#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

from world.objects.Thing import Thing
from common import const

class Fire(Thing):
    """fire to burn things up"""
    #CHEAT! make it more realistic (like spreading to things that burn near)
    def extinguish_operation(self, op):
        """If somebody tries to extinguish us, change status lower"""
        self.status=self.status-0.1
        self_ent=Entity(self.id,status=self.status)
        #print "Extinguish:",self,self.location.parent,self.status,op.from_
        return Operation("set",self_ent,to=self)
    def tick_operation(self, op):
        """things to do every tick, see comments"""
        #print `self`,"Got tick operation:\n",op
        #Is fire extinguished?
        if self.status<0.0:
            return Operation("delete",Entity(self.id),to=self)

        #Have we burned up our parent container?
        if self.world.is_object_deleted(self.location.parent) or \
           self.world==self.location.parent:
            return Operation("delete",Entity(self.id),to=self)

        #Send fire operation to parent container
        self_ent=Entity(self.id,status=self.status)
        opFire=Operation("fire",self_ent,to=self.location.parent)
        if self.status<0.2:
            self.status=self.status-0.001
        else:
            if self.status<1.0:
                self.status=self.status-0.005
            else:
                self.status=self.status-0.01
        self_ent2=Entity(self.id,status=self.status)
        opSet=Operation("set",self_ent2,to=self)

        #and call this method later
        opTick=Operation("tick",to=self)
        opTick.time.sadd=const.basic_tick*4
        return Message(opFire,Operation("sight",opFire,to='all'),opTick,opSet)
    def nourish_operation(self, op):
        #Increase fire
        inc=0.1
        if len(op) > 1:
            inc=op[1].weight
        self.status=self.status+inc
        #No need to send a set. The next tick will deal with that.
    def move_operation(self, op): pass
