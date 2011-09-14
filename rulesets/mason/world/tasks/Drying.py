#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Vector3D

import server

class Drying(server.Task):
    """ A task for Drying grass to fibre."""
    def cut_operation(self, op):
        """ Op handler for cut op which activates this task """
        # print "Drying.cut"

        if len(op) < 1:
            sys.stderr.write("Drying task has no target in cut op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to


    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Drying.tick"

        target=server.world.get_object(self.target)
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        self.rate = 0.2 / 0.75
        self.progress += 0.5

        if square_distance(self.character.location, target.location) > target.location.bbox.square_bounding_radius():
            self.rate = 0
            # print "Too far away"
            return self.next_tick(0.75)

        res=Oplist()

        if self.progress < 1:
            # print "Not done yet"
            return self.next_tick(0.75)

        self.progress = 0

        chunk_loc = target.location.copy()

        chunk_loc.coordinates = target.location.coordinates

        chunk_loc.orientation = target.location.orientation

        create=Operation("create",
                         Entity(name = "fibre",
                                type = "fibre",
                                location = chunk_loc), to = target)
        res.append(create)

        set = Operation("set", Entity(target.id, status = -1), to = target)
        res.append(set)

        res.append(self.next_tick(0.75))

        return res
