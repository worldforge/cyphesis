# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *

# class Slice(server.Task):
#     """ A task for cutting a log into boards."""
#
#     def plane_operation(self, op):
#         """ Op handler for cut op which activates this task """
#         # print "Slice.cut"
#
#         if len(op) < 1:
#             sys.stderr.write("Slice task has no target in cut op")
#
#         # FIXME Use weak references, once we have them
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#         self.width = 0.2
#
#     def tick_operation(self, op):
#         """ Op handler for regular tick op """
#         # print "Slice.tick"
#
#         target = self.target()
#         if target is None:
#             # print "Target is no more"
#             self.irrelevant()
#             return
#
#         if square_distance(self.character.location, target.location) > target.location.bbox.square_bounding_radius():
#             self.rate = 0
#             # print "Too far away"
#             return self.next_tick(0.75)
#         else:
#             self.progress += 0.5
#
#         self.rate = 0.5 / 0.75
#
#         if self.progress < 1:
#             # print "Not done yet"
#             return self.next_tick(0.75)
#
#         width = target.location.bbox.high_corner.x - target.location.bbox.low_corner.x
#         if width <= self.width:
#             # print "Nothing more to cut"
#             self.irrelevant()
#             return
#
#         res = Oplist()
#
#         new_bbox = [target.location.bbox.low_corner.x,
#                     target.location.bbox.low_corner.y,
#                     target.location.bbox.low_corner.z,
#                     target.location.bbox.high_corner.x - self.width,
#                     target.location.bbox.high_corner.y,
#                     target.location.bbox.high_corner.z]
#
#         set = Operation("set", Entity(target.id, bbox=new_bbox), to=target)
#         res.append(set)
#
#         slice_loc = target.location.copy()
#
#         pos_offset = Vector3D(target.location.bbox.high_corner.x, 0, 0)
#         pos_offset.rotate(target.location.orientation)
#
#         slice_loc.pos = target.location.pos + pos_offset
#
#         slice_bbox = [0,
#                       target.location.bbox.low_corner.y,
#                       target.location.bbox.low_corner.z,
#                       self.width,
#                       target.location.bbox.high_corner.y,
#                       target.location.bbox.high_corner.z]
#
#         slice_loc.orientation = target.location.orientation
#
#         create = Operation("create", Entity(name='wood', type='wood', location=slice_loc, bbox=slice_bbox), to=target)
#         res.append(create)
#
#         if width - self.width > self.width:
#             self.progress = 0
#         else:
#             # FIXME Integrate with other set op.
#             # Add create to convert remaining fragment into a board
#             set = Operation("set", Entity(target.id, status=-1), to=target)
#             res.append(set)
#
#         res.append(self.next_tick(0.75))
#
#         return res
