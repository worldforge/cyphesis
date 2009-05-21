#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Point3D
from physics import Vector3D

from cyphesis.Thing import Thing

import server

class Delve(Thing):
    """ A task for cutting a log into boards."""

    materials = { 0: 'boulder', 4: 'ice' }
    def cut_operation(self, op):
        """ Op handler for cut op which activates this task """
        # print "Delve.cut"

        if len(op) < 1:
            sys.stderr.write("Delve task has no target in cut op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

        self.pos = Point3D(op[0].pos)

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Delve.tick"

        target=server.world.get_object(self.target)
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        old_rate = self.rate

        self.rate = 0.1 / 1.75
        self.progress += 0.1

        if old_rate < 0.01:
            self.progress = 0
        else:
            self.progress += 0.1

        # print "%s" % self.pos

        if self.progress < 1:
            # print "Not done yet"
            return self.next_tick(1.75)

        self.progress = 0

        surface = target.terrain.get_surface(self.pos)
        # print "SURFACE %d at %s" % (surface, self.pos)
        if surface not in Delve.materials:
            # print "Not rock"
            self.irrelevant()
            return

        res=Oplist()

        chunk_loc = Location(self.character.location.parent)
        chunk_loc.velocity = Vector3D()

        chunk_loc.coordinates = self.pos

        create=Operation("create",
                         Entity(name = Delve.materials[surface],
                                type = Delve.materials[surface],
                                location = chunk_loc), to = target)
        res.append(create)

        res.append(self.next_tick(1.75))

        return res
