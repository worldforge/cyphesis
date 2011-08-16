#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Point3D
from physics import Vector3D

import server

class Dragging(server.Task):
    """ A task for dragging heavy objects by using a pulley ."""

    materials = { 0: 'wood' }
    def drag_operation(self, op):
        """ Op handler for cut op which activates this task """
        # print "Dragging.drag"

        if len(op) < 1:
            sys.stderr.write("Dragging task has no target in drag op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

        self.pos = Point3D(op[0].pos)

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Dragging.tick"
        target=server.world.get_object(self.target)
        self.pos = self.character.location.coordinates
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return


        old_rate = self.rate

        self.rate = 0.5 / 0.75
        self.progress += 0.5

        if old_rate < 0.01:
            self.progress = 0

        # print "%s" % self.pos

        if self.progress < 1:
            # print "Not done yet"
            return self.next_tick(0.75)

        self.progress = 0

        res=Oplist()

        chunk_loc = Location(self.character.location.parent)
        chunk_loc.velocity = Vector3D()

        chunk_loc.coordinates = self.pos

        res = res + Operation("move", Entity(target.id, location = chunk_loc), to = target)
        res.append(self.next_tick(0.75))

        return res
