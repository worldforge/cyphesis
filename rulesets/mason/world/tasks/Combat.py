#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005 Al Riddoch (See the file COPYING for details).

from atlas import *

from world.objects.Thing import Thing

class Combat(Thing):
    """The mason combat system."""
    def __init__(self, cppthing):
        self.cinit(cppthing)
        self.attackers=[]
        self.defenders=[]
        print "Task Created"
    def attack_operation(self, op):
        print "Got attack"
        defender = op.to
        if defender != self.character.id:
            print "Attach operation is wierd"
        if defender not in self.defenders:
            print "New defender"
            self.defenders.append(defender)
        print "Attacker", op[0].id
        attacker = op[0].id
        if attacker not in self.attackers:
            print "New attacker"
            self.attackers.append(attacker)
        res=Operation("tick", to=self.character.id)
        res.sub_to="task"
        return res
    def tick_operation(self, op):
        print "Got tick"
        res=Operation("tick", to=self.character.id)
        res.sub_to="task"
        res.setFutureSeconds(1.75)
        return res
