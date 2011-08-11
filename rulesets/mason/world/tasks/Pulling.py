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
        self.target = op[0].id
        self.tool = op.to
        self.points = []
        self.points.append(self.character.location.coordinates)

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        res = Oplist()

        target=server.world.get_object(self.target)
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        if not target.location.parent:
            self.irrelevant()
            return

        target_location = Location(target.location.parent, target.location.coordinates)
	print target.location.parent
	print target.location.coordinates
	print target_location
        target_location.velocity=Vector3D(0,0,-0.5)
	print "self"
	print self.character.location.coordinates
        new_loc = self.character.location.coordinates
        origin = self.points[0]
        diff = origin.x - new_loc.x
        target_entity_moving = Entity(self.target, location = target_location)

        target_location = Location(target.location.parent, Point3D(target.location.coordinates.x, target.location.coordinates.y, target.location.coordinates.z + diff))
        target_location.velocity=Vector3D(0,0,0)
        target_entity = Entity(self.target, location = target_location)

        if not hasattr(target, 'mode') or target.mode != 'fixed':
            target_entity.mode = 'fixed'

        move=Operation("move", target_entity, to=self.target)
        move.setFutureSeconds(0.2)
        res.append(move)

        res.append(self.next_tick(1.5))

        return res

