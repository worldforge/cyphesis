#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from world.objects.Thing import Thing
from common import log,const
from misc import set_kw

# Rate at which energy is used per tick
energyConsumption = 0.01
# Rate at which food boosts energy
foodConsumption = 0.1
# Weight lost when energy is exhausted
weightConsumption = 1.0
# Energy gained when weight is lost
energyGain = 0.5
# Excess energy that is to be turned into weight
energyLoss = 0.1
# Weight gained from the excess energy
weightGain = 0.5

class Animal(Thing):
    """This is base class for all kind of animals"""
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
        set_kw(self,kw,"status",1.0)
        set_kw(self,kw,"weight",1.0)
        self.food=0
        self.maxweight=50
        self.tickcount=0
    def tick_operation(self, op):
        """check energy state and see if we are starving"""
        #CHEAT!: fix this with changing tick model to:
        #use tick_list in worldrouter where it's stored next tick for each
        #object
        #print self, self.status, self.weight
        res = Message()
        res = res + Character.tick_operation(self, op)
        self.tickcount=self.tickcount+1
        if self.tickcount<30:
            return res
        self.tickcount=0
##         if not res: 
##             res = Operation("tick",to=self)
##             res.time.sadd = const.basic_tick
        ent = Entity(self.id)
        if self.food>=foodConsumption and self.status < 2.0:
            self.status = self.status + foodConsumption
            self.food = self.food - foodConsumption
            #Send a private sight op to ourselves so our mind know how much food we have
            res.append(Operation("sight", Operation("set", Entity(self.id, food=self.food)), to=self))
        if self.status>(1.5+energyLoss) and self.weight < self.maxweight:
            self.status = self.status - energyLoss
            ent.weight = self.weight + weightGain
        if self.status<=energyConsumption and self.weight>weightConsumption:
            ent.status = self.status - energyConsumption + energyGain
            ent.weight = self.weight - weightConsumption
        else:
            ent.status = self.status - energyConsumption
        res.append(Operation("set", ent, to = self))
        return res
    def nourish_operation(self, op):
        weight=1
        if len(op) > 1:
            weight=op[1].weight
        self.food = self.food + weight
        #ent = Entity(self.id)
        #ent.status = self.status + energyGain
        #return Operation("set", ent, to = self)
        #Send a private sight op to ourselves so our mind know how much food we have
        return Operation("sight", Operation("set", Entity(self.id, food=self.food)), to=self)
    def eat_operation(self, op):
        ent=Entity(self.id,status=-1)
        res = Operation("set",ent,to=self)
        to_ = self.world.get_object(op[1].id)
        if not to_:
            return self.error(op,"To is undefined object")
        ent.weight=self.weight
        res = res + Operation("nourish",op[1],ent,to=to_,from_=self)
        return res
