# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *

#
# class Destroying(server.Task):
#     """A very simple Destroy system for destroying structures."""
#
# #TODO: update, as "attack" has been removed
#     def attack_operation(self, op):
#         """ The attack op is FROM the the character,
#             TO the structure that is getting destroyed which we
#             term the target. """
#
#         if len(op) < 1:
#             sys.stderr.write("Destroy task has no target in attack op")
#
#         # FIXME Use weak references, once we have them
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#     def tick_operation(self, op):
#         """ This method is called repeatedly, each time a Destroy turn occurs.
#             In this example the interval is fixed, but it can be varied. """
#         # print "Destroy.tick"
#
#         if self.target() is None:
#             # print "Target is no more"
#             self.irrelevant()
#             return
#
#         if self.character.stamina <= 0:
#             # print "I am exhausted"
#             self.irrelevant()
#             return
#
#         res = Oplist()
#         chunk_loc = self.target().location.copy()
#         chunk_loc.pos = self.target().location.pos
#         chunk_loc.orientation = self.target().location.orientation
#         # Some entity do not have status defined. If not present we assume that the entity is unharmed
#         if hasattr(self.target().props, 'status'):
#             current_status = self.target().props.status
#
#         else:
#             set = Operation("set", Entity(self.target().id, status=1),
#                             to=self.target())
#             res.append(set)
#             current_status = 1.0
#
#         # Measure the distance between the entity horizontal edges. Else we won't be able to reach if either entity is too thick.
#         distance_between_entity_edges_squared = square_horizontal_edge_distance(self.character.location, self.target().location)
#
#         # Assume that a standard human can reach 1.5 meters, and use this to determine if we're close enough to be able to perform the logging
#         standard_human_reach_squared = 1.5 * 1.5
#
#         if distance_between_entity_edges_squared > standard_human_reach_squared:
#             self.progress = 1 - current_status
#             self.rate = 0
#             return self.next_tick(1.75)
#
#         if current_status > 0.11:
#             set = Operation("set", Entity(self.target().id, status=current_status - 0.1), to=self.target())
#             res.append(set)
#
#         else:
#             # Creating ruins when a particular entity gets destroyed.
#             if self.target().type[0] == "castle_house_a":
#                 create = Operation("create", Entity(name="castle_house_ruin", type="castle_house_ruin", location=chunk_loc), to=self.target())
#                 res.append(create)
#
#             set = Operation("set", Entity(self.target().id, status=-1),
#                             to=self.target())
#             res.append(set)
#             self.irrelevant()
#
#         self.progress = 1 - current_status
#         self.rate = 0.1 / 1.75
#
#         res.append(self.next_tick(1.75))
#
#         return res
