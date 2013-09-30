#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Vector3D
from physics import Point3D

from random import *

import server

class Ram(server.Task):
    """ A proof of concept task for drivinf object into the ground."""
    def strike_operation(self, op):
        """ Op handler for strike op which activates this task """
        if len(op) < 1:
            std.stderr.write("Ram task has no target in strike op")

        # FIXME Use weak references, once we have them
        self.target = server.world.get_object_ref(op[0].id)
        self.tool = op.to
    def tick_operation(self, op):
        """ Op handler for regular tick op """
        res = Oplist()

        if self.target() is None:
            # print "Target is no more"
            self.irrelevant()
            return

        if not self.target().location.parent:
            self.irrelevant()
            return

        long_var = self.target().location.bbox.square_bounding_radius()
        if square_distance(self.character.location,
                           self.target().location) > long_var:
            return self.next_tick(1)

        target_location = Location(self.target().location.parent, self.target().location.coordinates)
        target_location.velocity=Vector3D(0,0,-0.5)
        target_entity_moving = Entity(self.target().id, location = target_location)

        target_location = Location(self.target().location.parent,
                                   Point3D(self.target().location.coordinates.x,
                                   self.target().location.coordinates.y,
                                   self.target().location.coordinates.z - 0.1))
        target_location.velocity=Vector3D(0,0,0)
        target_entity = Entity(self.target().id, location = target_location)

        if not hasattr(self.target(), 'mode') or self.target().mode != 'fixed':
            target_entity.mode = 'fixed'

        move=Operation("move", target_entity, to=self.target())
        move.setFutureSeconds(0.2)
        res.append(move)
        move=Operation("move", target_entity_moving, to=self.target())
        res.append(move)

        res.append(self.next_tick(1))

        return res

