# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *

#
# class Ram(server.Task):
#     """ A proof of concept task for drivinf object into the ground."""
#
#     def strike_operation(self, op):
#         """ Op handler for strike op which activates this task """
#         if len(op) < 1:
#             std.stderr.write("Ram task has no target in strike op")
#
#         # FIXME Use weak references, once we have them
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#     def tick_operation(self, op):
#         """ Op handler for regular tick op """
#         res = Oplist()
#
#         if self.target() is None:
#             # print "Target is no more"
#             self.irrelevant()
#             return
#
#         if not self.target().parent:
#             self.irrelevant()
#             return
#
#         if square_distance(self.character.location, self.target().location) > self.target().location.bbox.square_bounding_radius():
#             return self.next_tick(1)
#
#         target_location = Location(self.target().parent, self.target().location.pos)
#         target_location.velocity = Vector3D(0, -0.5, 0)
#         target_entity_moving = Entity(self.target().id, location=target_location)
#
#         target_location = Location(self.target().parent, Point3D(self.target().location.pos.x, self.target().location.pos.y - 0.1, self.target().location.pos.z))
#         target_location.velocity = Vector3D(0, 0, 0)
#         target_entity = Entity(self.target().id, location=target_location)
#
#         if not hasattr(self.target(), 'mode') or self.target().props.mode != 'fixed':
#             target_entity.mode = 'fixed'
#
#         move = Operation("move", target_entity, to=self.target())
#         move.set_future_seconds(0.2)
#         res.append(move)
#         move = Operation("move", target_entity_moving, to=self.target())
#         res.append(move)
#
#         res.append(self.next_tick(1))
#
#         return res
