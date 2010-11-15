#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Vector3D

from random import *

import server

class _Zone(object):
    def __init__(self):
        print "Creating magic zone"

    def get_power(self, pos):
        """Determine the spell power at a location"""
        # Put maths or lookup code here to work out what the "wave amplitude"
        # is at pos.
        return 1

class Spell(server.Task):
    """ A proof of concept task for making new paths and roads."""
    zone = None
    def cast_operation(self, op):
        """ Op handler for strike op which activates this task """
        print "Spell.cast"

        if len(op) < 1:
            sys.stderr.write("Spell task has no target in cast op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

        if not Spell.zone:
            Spell.zone = _Zone()
        self.things = 'things'
        self.points = []

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        print "Spell.tick"
        res=Oplist()

        target=server.world.get_object(self.target)
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        # Get the amount of magic power available to this spell from the zone
        spell_power = self.zone.get_power(self.character.location.coordinates)

        return self.next_tick(1.75)
