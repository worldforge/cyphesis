#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from Quaternion import Quaternion
from Vector3D import Vector3D

try:
  from random import *
except ImportError:
  from whrandom import *

from world.objects.Thing import Thing

class Raise(Thing):
    """ A proof of concept task for raiseing a heavy item from the ground."""
    def lever_operation(self, op):
        """ Op handler for lever op which activates this task """
        print "Raise.lever"

        if len(op) < 1:
            print "No target"

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        print "Raise.tick"
        res=Message()

        target=self.character.world.get_object(self.target)
        if not target:
            print "Target is no more"
            self.irrelevant()
            return

        distance=distance_to(self.character.location, target.location)
        axis=distance.cross(Vector3D(0,0,1))
        print "DISTANCE ", distance, axis
        rotation=Quaternion(axis, -0.01)
        print "ROT ", rotation
        if target.location.orientation.valid():
            rotation = rotation * target.location.orientation

        if not target.location.parent:
            # Not withstanding famous quotes to the contrary, in these
            # system we can not move the world no matter how large a lever
            # or firm a place to stand we have.
            return
        target_location=Location(target.location.parent, target.location.coordinates)
        target_location.orientation=rotation
        move=Operation("move", Entity(self.target, location=target_location), to=self.target)
        res.append(move)

        tick=Operation("tick", to=self.character.id)
        tick.setFutureSeconds(1)
        tick.sub_to="task"
        res.append(tick)

        return res
