#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005 Al Riddoch (See the file COPYING for details).

from world.objects.Thing import Thing

class Combat(Thing):
    """The mason combat system."""
    def __init__(self, cppthing):
        self.cinit(cppthing)
        print "Task Created"
    def attack_operation(self, op):
        print "Got attack"
    def tick_operation(self, op):
        print "Got tick"
