# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2011 Peter  <elminister@interia.pl> (See the file COPYING for details).

# NOT YET READY

from atlas import *
from physics import *

# class BuildBloomery(server.Task):
#     """A task for creating a Bloomery by consuming 1 charcoal, 1 hematite and 1 pile of earth"""
#
#     def earthwork_bloomery_operation(self, op):
#         """ Op handler for bloomery op which activates this task """
#
#         if len(op) < 1:
#             sys.stderr.write("Bloomery_build task has no target in bloomery_build op")
#
#         # FIXME Use weak references, once we have them
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#         self.pos = Point3D(op[0].pos)
#
#     def tick_operation(self, op):
#
#         """ Op handler for regular tick op """
#         target = self.target()
#         if not target:
#             # print "Target is no more"
#             self.irrelevant()
#             return
#
#         self.rate = 0.5 / 0.75
#         self.progress += 1
#
#         if not target:
#             print("Target is no more")
#             self.irrelevant()
#             return
#
#         res = Oplist()
#
#         if self.progress < 1:
#             # print "Not done yet"
#             return self.next_tick(0.75)
#
#         self.progress = 0
#         # counter for charcal, hematite and pilef of earth
#         charcoal_count = 0
#         hematite_count = 0
#         earth_count = 0
#         count = 0
#         # List which stores the to be consumed entities
#         raw_materials = []
#
#         # Make sure only 1 part of each attribute is being consumed as per the recipe.
#         for item in self.character.contains:
#             if item.type[0] == str("hematite"):
#                 if hematite_count <= 0:
#                     raw_materials.append(item)
#                     hematite_count = hematite_count + 1
#             if item.type[0] == str("charcoal"):
#                 if charcoal_count <= 0:
#                     raw_materials.append(item)
#                     charcoal_count = charcoal_count + 1
#             # we accept only piles of earth not dirt or some other shit
#             if item.type[0] == str("pile") and item.material == str("earth"):
#                 print("Pile of earth was found in inventory")
#                 if earth_count <= 0:
#                     raw_materials.append(item)
#                     earth_count = earth_count + 1
#             if (hematite_count + charcoal_count + earth_count) == 3:
#                 break
#         else:
#             print("No materials in inventory")
#             self.irrelevant()
#             return
#
#         chunk_loc = target.location.copy()
#         chunk_loc.pos = target.location.pos
#         chunk_loc.orientation = target.location.orientation
#
#         count = earth_count + charcoal_count + hematite_count
#
#         # consume the materials stores in the list raw_materials
#         while (count > 0):
#             tar = raw_materials.pop()
#             set = Operation("set", Entity(tar.id, status=-1), to=tar)
#             res.append(set)
#             count = count - 1
#
#         create = Operation("create", Entity(name="bloomery", type="bloomery", location=chunk_loc), to=target)
#         res.append(create)
#         self.progress = 1
#         self.irrelevant()
#         return res
