#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Vector3D
from physics import Point3D

from random import *

import server

class Pulling(server.Task):
    """ A task for pulling objects vertically using simple rope mechanism."""
    def pull_operation(self, op):
        """ Op handler for strike op which activates this task """
        if len(op) < 1:
            std.stderr.write("Pulling task has no target in pull op")

        # FIXME Use weak references, once we have them
        self.target = server.world.get_object(op[0].id)
        self.tool = op.to
        self.points = []
        self.points.append(self.character.location.position)

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

        target_location = Location(self.target().location.parent,
                                   self.target().location.position)
        target_location.velocity=Vector3D(0, -0.5, 0)
        new_loc = self.character.location.position
        origin = self.points[0]
        # Get the diffrence in the location of user at current time to the time when he started the task
        diff = origin.x - new_loc.x
        target_entity_moving = Entity(self.target().id, location = target_location)

        # Replicate the diffrence in position to the corresponding change in height.
        target_location = Location(self.target().location.parent,
                                   Point3D(self.target().location.position.x, 
                                   self.target().location.position.y + diff,
                                   self.target().location.position.z))
        target_location.velocity=Vector3D(0,0,0)
        target_entity = Entity(self.target().id, location = target_location)

        # Make the mode fixed to remove the height constraint on entity. 
        if not hasattr(self.target(), 'mode') or self.target().props.mode != 'fixed':
            target_entity.mode = 'fixed'

        move=Operation("move", target_entity, to=self.target())
        move.setFutureSeconds(0.2)
        res.append(move)

        res.append(self.next_tick(1.5))

        return res

