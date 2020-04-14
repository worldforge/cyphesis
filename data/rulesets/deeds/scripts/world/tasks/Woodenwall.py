# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *

# class Woodenwall(server.Task):
#     """A task for creating a Woodenwall by consuming 1 lumber , 1 wooden board and a rope"""
#
#     materials = ["lumber", "wood"]
#
#     def walls_operation(self, op):
#         """ Op handler for walls op which activates this task """
#
#         if len(op) < 1:
#             sys.stderr.write("Woodenwall task has no target in walls op")
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
#         # counter for lumber, wood entity and total of previous one
#         lcount = 0
#         wcount = 0
#         count = 0
#         # List which stores the to be consumed entity
#         raw_materials = []
#
#         # Make sure only 1 part of each attribute is being consumed as per the recipe.
#         for item in self.character.contains:
#             if item.type[0] == str(self.materials[0]):
#                 if lcount == 0:
#                     raw_materials.append(item)
#                     lcount = lcount + 1
#             if item.type[0] == str(self.materials[1]):
#                 if wcount == 0:
#                     raw_materials.append(item)
#                     wcount = wcount + 1
#             if (lcount + wcount) == 2:
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
#         count = lcount + wcount
#
#         # consume the materials stores in the list raw_materials
#         while (count > 0):
#             tar = raw_materials.pop()
#             set = Operation("set", Entity(tar.id, status=-1), to=tar)
#             res.append(set)
#             count = count - 1
#
#         create = Operation("create", Entity(name="board_wall", type="board_wall",
#                                             location=chunk_loc), to=target)
#         res.append(create)
#         self.progress = 1
#         self.irrelevant()
#         return res
