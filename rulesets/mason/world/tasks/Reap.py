#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from Quaternion import Quaternion
from physics import Vector3D

from cyphesis.Thing import Thing

import server

class Reap(Thing):
    """ A task for cutting a log into boards."""
    def cut_operation(self, op):
        """ Op handler for cut op which activates this task """
        # print "Reap.cut"

        if len(op) < 1:
            sys.stderr.write("Reap task has no target in cut op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Reap.tick"

        target=server.world.get_object(self.target)
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        if self.count == 0:
            self.count = int(target.mass)
            # print "setting target mass to ", self.count


        if not self.character.location.velocity.is_valid() or \
           self.character.location.velocity.square_mag() < 1 or \
           self.character.location.velocity.square_mag() > 5:
            self.rate = 0
            self.progress = 0
            # print "Not moving the right speed"
            return self.next_tick(1.75)

        old_rate = self.rate

        self.rate = 1.0 / 1.75
        self.progress = 0.01

        if old_rate < 0.1:
            # print "Wasn't moving right speed"
            return self.next_tick(1.75)

        surface = target.terrain.get_surface(self.character.location.coordinates)
        if surface is not 2:
            # print "Not grass"
            return self.next_tick(1.75)

        res=Message()

        chunk_loc = Location(self.character.location.parent)
        chunk_loc.velocity = Vector3D()

        chunk_loc.coordinates = self.character.location.coordinates

        create=Operation("create",
                         Entity(name = "grass",
                                type = "grass",
                                location = chunk_loc), to = target)
        res.append(create)

        res.append(self.next_tick(1.75))

        return res
