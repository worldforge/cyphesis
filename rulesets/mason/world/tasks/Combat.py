#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005 Al Riddoch (See the file COPYING for details).

from atlas import *

from world.objects.Thing import Thing

class Combat(Thing):
    """The mason combat system."""
    def __init__(self, cppthing):
        self.cinit(cppthing)
        print "Task Created"
    def attack_operation(self, op):
        print "Got attack"
        res=Operation("tick", to=self.character.id)
        res.sub_to="task"
        return res
    def tick_operation(self, op):
        print "Got tick"
        res=Operation("tick", to=self.character.id)
        res.sub_to="task"
        res.setFutureSeconds(1.75)
        return res
