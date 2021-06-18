# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *

#
# class Raise(server.Task):
#     """ A proof of concept task for raiseing a heavy item from the ground."""
#
#     def lever_operation(self, op):
#         """ Op handler for lever op which activates this task """
#         # print "Raise.lever"
#
#         if len(op) < 1:
#             std.stderr.write("Raise task hsa no target in lever op")
#
#         # FIXME Use weak references, once we have them
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#     def tick_operation(self, op):
#         """ Op handler for regular tick op """
#         # print "Raise.tick"
#
#         if self.target() is None:
#             # print "Target is no more"
#             self.irrelevant()
#             return
#         if not self.target().parent:
#             # Not withstanding famous quotes to the contrary, in these
#             # system we can not move the world no matter how large a lever
#             # or firm a place to stand we have.
#             self.irrelevant()
#             return
#
#         distance = distance_to(self.character.location, self.target().location)
#         # Check we are not too far away from the object to interact with it
#         # This calculation is imprecise as it sums the square radii, but
#         # its usually close enough.
#         d = distance.sqr_mag()
#         r = self.character.location.bbox.square_bounding_radius() + \
#             self.target().location.bbox.square_bounding_radius()
#         if d > r:
#             return self.next_tick(1);
#         if d < const.epsilon:
#             # print "Going nowhere"
#             return self.next_tick(1);
#         # print "DISTANCE ", distance, distance.is_valid()
#         axis = distance.cross(Vector3D(0, 1, 0))
#         # If distance is zero, axis becomes zero
#         # print "DISTANCE ", distance, distance.is_valid(), axis, axis.is_valid()
#         # If axis is zero, the quaternion contains NaNs.
#         rotation = Quaternion(axis, -0.05)
#         # print "ROT ", rotation, rotation.is_valid()
#         if self.target().location.orientation.is_valid():
#             # print "VALID"
#             rotation = self.target().location.orientation * rotation
#
#         # print "NEW_ROT", rotation, rotation.is_valid()
#         target_location = Location(self.target().parent,
#                                    self.target().location.pos)
#         target_location.orientation = rotation
#         move = Operation("move", Entity(self.target().id,
#                                         location=target_location), to=self.target())
#         res = Oplist()
#         res.append(move)
#
#         res.append(self.next_tick(0.5))
#
#         return res
