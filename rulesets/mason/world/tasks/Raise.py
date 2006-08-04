#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from Quaternion import Quaternion
from Vector3D import Vector3D

from common import const

try:
  from random import *
except ImportError:
  from whrandom import *

from world.objects.Thing import Thing

class Raise(Thing):
    """ A proof of concept task for raiseing a heavy item from the ground."""
    def lever_operation(self, op):
        """ Op handler for lever op which activates this task """
        # print "Raise.lever"

        if len(op) < 1:
            std.stderr.write("Raise task hsa no target in lever op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Raise.tick"

        target=self.character.world.get_object(self.target)
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return
        if not target.location.parent:
            # Not withstanding famous quotes to the contrary, in these
            # system we can not move the world no matter how large a lever
            # or firm a place to stand we have.
            self.irrelevant()
            return

        distance=distance_to(self.character.location, target.location)
        # Check we are not too far away from the object to interact with it
        # This calculation is imprecise as it sums the square radii, but
        # its usually close enough.
        d = distance.square_mag()
        r = self.character.location.bbox.square_bounding_radius() + target.location.bbox.square_bounding_radius()
        if d > r:
            return self.next_tick(1);
        if d < const.epsilon:
            # print "Going nowhere"
            return self.next_tick(1);
        # print "DISTANCE ", distance, distance.is_valid()
        axis=distance.cross(Vector3D(0,0,1))
        # If distance is zero, axis becomes zero
        # print "DISTANCE ", distance, distance.is_valid(), axis, axis.is_valid()
        # If axis is zero, the quaternion contains NaNs.
        rotation=Quaternion(axis, -0.01)
        # print "ROT ", rotation, rotation.valid()
        if target.location.orientation.valid():
            # print "VALID"
            rotation = target.location.orientation * rotation

        # print "NEW_ROT", rotation, rotation.valid()
        target_location=Location(target.location.parent, target.location.coordinates)
        target_location.orientation=rotation
        move=Operation("move", Entity(self.target, location=target_location), to=self.target)
        res=Message()
        res.append(move)

        res.append(self.next_tick(1))

        return res
