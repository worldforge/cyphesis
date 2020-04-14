# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *

#
# class Sharpen(server.Task):
#     """ A proof of concept task for sharpening a log into a stake."""
#
#     def cut_operation(self, op):
#         """ Op handler for cut op which activates this task """
#         # print "Sharpen.cut"
#
#         if len(op) < 1:
#             sys.stderr.write("Sharpen task has no target in cut op")
#
#         # FIXME Use weak references, once we have them
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#     def tick_operation(self, op):
#         """ Op handler for regular tick op """
#         # print "Sharpen.tick"
#         res = Oplist()
#
#         if self.target() is None:
#             # print "Target is no more"
#             self.irrelevant()
#             return
#
#         new_status = 1
#         if hasattr(self.target(), 'status'):
#             new_status = self.target().props.status - 0.1
#
#         # Measure the distance between the entity horizontal edges. Else we won't be able to reach if either entity is too thick.
#         distance_between_entity_edges_squared = square_horizontal_edge_distance(self.character.location, self.target().location)
#
#         # Assume that a standard human can reach 1.5 meters, and use this to determine if we're close enough to be able to perform the logging
#         standard_human_reach_squared = 1.5 * 1.5
#
#         if distance_between_entity_edges_squared > standard_human_reach_squared:
#             self.progress = 1 - new_status
#             self.rate = 0
#             return self.next_tick(1.75)
#
#         if new_status < 0.1:
#             new_status = -1
#             new_loc = self.target().location.copy()
#             new_loc.bbox = self.target().location.bbox
#             new_loc.orientation = self.target().location.orientation
#             create = Operation("create", Entity(name='stake', type='stake', location=new_loc), to=self.target())
#             res.append(create)
#
#         set = Operation("set", Entity(self.target().id, status=new_status), to=self.target())
#         res.append(set)
#
#         self.progress = 1 - new_status
#         self.rate = 0.1 / 1.75
#
#         res.append(self.next_tick(1.75))
#
#         return res
