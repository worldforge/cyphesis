# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *

# class Earthwall(server.Task):
#     """A task for creating a Earthwall by consuming one part boulder , 1 part earth and 1 part sand """
#
#     materials = ["pile", "boulder"]
#
#     def walls_operation(self, op):
#         """ Op handler for walls op which activates this task """
#
#         if len(op) < 1:
#             sys.stderr.write("Earthwall task has no target in walls op")
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
#         if self.target() is None:
#             # print "Target is no more"
#             self.irrelevant()
#             return
#
#         self.rate = 0.5 / 0.75
#         self.progress += 1
#
#         res = Oplist()
#
#         if self.progress < 1:
#             # print "Not done yet"
#             return self.next_tick(0.75)
#
#         self.progress = 0
#         # counter for Earth , sand , boulder entity and total of previous one
#         ecount = 0
#         scount = 0
#         bcount = 0
#         count = 0
#         # List which stores the to be consumed entity
#         raw_materials = []
#         # Make sure only 1 part of each attribute is being consumed as per the recipe.
#         for item in self.character.contains:
#             if item.type[0] == str(self.materials[0]):
#                 if item.name == "earth":
#                     if ecount == 0:
#                         raw_materials.append(item)
#                         ecount = ecount + 1
#                 if item.name == "sand":
#                     if scount == 0:
#                         raw_materials.append(item)
#                         scount = scount + 1
#             if item.type[0] == str(self.materials[1]):
#                 if bcount == 0:
#                     raw_materials.append(item)
#                     bcount = bcount + 1
#             if (ecount + scount + bcount) == 3:
#                 break
#         else:
#             print("No materials in inventory")
#             self.irrelevant()
#             return
#
#         chunk_loc = self.target().location.copy()
#         chunk_loc.pos = self.target().location.pos
#         chunk_loc.orientation = self.target().location.orientation
#
#         count = ecount + scount + bcount
#         # consume the materials stores in the list raw_materials
#         while (count > 0):
#             tar = raw_materials.pop()
#             set = Operation("set", Entity(tar.id, status=-1), to=tar)
#             res.append(set)
#             count = count - 1
#
#         create = Operation("create", Entity(name="earth_wall",
#                                             type="earth_wall", location=chunk_loc),
#                            to=self.target())
#         res.append(create)
#         self.progress = 1
#         self.irrelevant()
#
#         return res
