#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from Quaternion import Quaternion
from Vector3D import Vector3D
from Point3D import Point3D

try:
  from random import *
except ImportError:
  from whrandom import *

from world.objects.Thing import Thing

class Ram(Thing):
    """ A proof of concept task for drivinf object into the ground."""
    def strike_operation(self, op):
        """ Op handler for strike op which activates this task """
        if len(op) < 1:
            std.stderr.write("Ram task has no target in strike op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to
    def tick_operation(self, op):
        """ Op handler for regular tick op """
        res = Message()

        target=self.character.world.get_object(self.target)
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        if not target.location.parent:
            return
        target_location = Location(target.location.parent, target.location.coordinates)
        target_location.velocity=Vector3D(0,0,-0.5)
        target_entity_moving = Entity(self.target, location = target_location)

        target_location = Location(target.location.parent, Point3D(target.location.coordinates.x, target.location.coordinates.y, target.location.coordinates.z - 0.1))
        target_location.velocity=Vector3D(0,0,0)
        target_entity = Entity(self.target, location = target_location)

        if not hasattr(target, 'mode') or target.mode != 'fixed':
            target_entity.mode = 'fixed'

        move=Operation("move", target_entity, to=self.target)
        move.setFutureSeconds(0.2)
        res.append(move)
        move=Operation("move", target_entity_moving, to=self.target)
        res.append(move)

        tick=Operation("tick", Entity(name="task",serialno=self.new_tick()), to=self.character.id)
        tick.setFutureSeconds(1)
        res.append(tick)

        return res

