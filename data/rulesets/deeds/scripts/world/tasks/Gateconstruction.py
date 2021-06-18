# This file is distributed under the terms of the GNU General
# Public license.
# Copyright (C) 2012 Anthony Pesce <timetopat@gmail.com>
# (See the file COPYING for details).

from atlas import *
from physics import *

#
# class Gateconstruction(server.Task):
#     """A task for creating a Gate using rope"""
#
#     materials = "lumber"
#
#     def gates_operation(self, op):
#         """ Op handler for Gateconstruction op which activates this task """
#
#         if len(op) < 1:
#             sys.stderr.write("Gateconstruction  task has no target in op")
#
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#         self.pos = Point3D(op[0].pos)
#
#     def info_operation(self, op):
#         print("Gate info")
#         gate = server.world.get_entity(op[0].id)
#         self.lcount = 0  # needs 1 lumber for basic gate
#         self.acount = 0  # needs 2 a frames for basic gate
#         raw_materials = []  # Holds construction
#         raw_materials1 = []  # Holds lumber
#         for item in self.character.contains:
#             if item.type[0] == str("construction"):
#                 raw_materials.append(item)
#                 self.acount = self.acount + 1
#             if item.type[0] == str("lumber"):
#                 raw_materials1.append(item)
#                 self.lcount = self.lcount + 1
#             if self.acount == 2 and self.lcount == 1:
#                 break
#             if self.lcount == 3:
#                 break
#         else:
#             print("No materials in inventory for Gate")
#             self.irrelevant()
#             return
#
#         chunk_loc = Location(gate())
#         chunk_loc.pos = Point3D([0, 0, 0])
#         res = Oplist()
#
#         if self.gname == "Basic_Gate":
#             count1 = self.lcount
#             while (count1 > 0):
#                 tar = raw_materials1.pop()
#                 self.lumber_length = tar.location.bbox.high_corner[2] - \
#                                      tar.location.bbox.low_corner[2]
#                 offset = Vector3D(self.lumber_length / 7, self.lumber_length * .63, -
#                 self.lumber_length / 3.5)
#                 chunk_loc.orientation = Quaternion([.707, 0, 0, .707])
#                 # Same as an Euler of (0,90 Degrees,0)
#                 chunk_loc.pos = chunk_loc.pos + offset
#                 move1 = Operation("move", Entity(tar.id, location=chunk_loc,
#                                                  mode="fixed"), to=tar)
#                 res.append(move1)
#                 count1 = count1 - 1
#
#             count = self.acount
#             chunk_loc = Location(gate())
#             chunk_loc.pos = Point3D([0, 0, 0])
#             # Resets
#             # loops through raw_materials and places 3 lumber
#             # in inventory infront of user
#             offset = Vector3D(0, 0, 0)
#             while (count > 0):
#                 tar = raw_materials.pop()
#
#                 if count == 2:
#                     # left component of gate
#                     chunk_loc.pos = Point3D([0, 0, 0])
#                     offset = Vector3D(0, self.lumber_length * .7, 0)
#                     chunk_loc.orientation = Quaternion([.707, 0, 0, .707])
#                     chunk_loc.pos = chunk_loc.pos + offset
#
#                 if count == 1:
#                     # right component of gate
#                     chunk_loc.pos = Point3D([0, 0, 0])
#                     offset = Vector3D(0,
#                                       self.lumber_length * .7, (self.lumber_length / 2))
#                     chunk_loc.orientation = Quaternion([.707, 0, 0, .707])
#                     chunk_loc.pos = chunk_loc.pos + offset
#
#                 move = Operation("move", Entity(tar.id, location=chunk_loc,
#                                                 mode="fixed"), to=tar)
#                 res.append(move)
#                 count = count - 1
#
#         if self.gname == "House_Gate":
#             # Left leg of the house frame
#             tar = raw_materials1.pop()
#             chunk_loc.pos = Point3D([0, 0, 0])
#             self.lumber_length = tar.location.bbox.high_corner[2] - \
#                                  tar.location.bbox.low_corner[2]
#             offset = Vector3D(0, self.lumber_length * .8, -self.lumber_length / 4)
#             chunk_loc.orientation = Quaternion([.707, .707, 0, 0])
#             chunk_loc.pos = chunk_loc.pos + offset
#             move1 = Operation("move", Entity(tar.id, location=chunk_loc,
#                                              mode="fixed"), to=tar)
#             res.append(move1)
#             # Right Leg of the house frame
#             tar = raw_materials1.pop()
#             chunk_loc.pos = Point3D([0, 0, 0])
#             self.lumber_length = tar.location.bbox.high_corner[2] - \
#                                  tar.location.bbox.low_corner[2]
#             offset = Vector3D(0,
#                               self.lumber_length * .8, -self.lumber_length * (3.0 / 4.0))
#             chunk_loc.orientation = Quaternion([.707, .707, 0, 0])
#             chunk_loc.pos = chunk_loc.pos + offset
#             move1 = Operation("move", Entity(tar.id, location=chunk_loc,
#                                              mode="fixed"), to=tar)
#             res.append(move1)
#             # Top of the house frame
#             tar = raw_materials1.pop()
#             chunk_loc.pos = Point3D([0, 0, 0])
#             self.lumber_length = tar.location.bbox.high_corner[2] - \
#                                  tar.location.bbox.low_corner[2]
#             offset = Vector3D(0, self.lumber_length * (.7, -self.lumber_length))
#             chunk_loc.orientation = Quaternion([.5, .5, -.5, .5])
#             # Same as (90 Degrees, 0, 90 Degrees)
#             chunk_loc.pos = chunk_loc.pos + offset
#             move1 = Operation("move", Entity(tar.id, location=chunk_loc,
#                                              mode="fixed"), to=tar)
#             res.append(move1)
#
#         self.progress = 1
#         self.irrelevant()
#         return res
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
#         lcount = 0  # Lumber count
#         acount = 0  # A frame count
#         self.gname = ""  # Gate name
#         # makes sure we have 3 lumber to construct A frame
#         for item in self.character.contains:
#             if item.type[0] == "lumber":
#                 lcount = lcount + 1
#             if item.type[0] == "construction":
#                 acount = acount + 1
#             if lcount == 3:
#                 self.gname = "House_Gate"
#                 break
#             if acount == 2 and lcount == 1:
#                 self.gname = "Basic_Gate"
#                 break
#         else:
#             print("No materials in inventory for a Gate")
#             self.irrelevant()
#
#         if self.progress < 1:
#             # print "Not done yet"
#             return self.next_tick(0.75)
#
#         self.progress = 0
#
#         chunk_loc = Location(self.character.parent)
#         chunk_loc.pos = self.pos
#         chunk_loc.orientation = self.character.location.orientation
#         res = Oplist()
#         bbox1 = [-4, -4, -.01, 4, 4, .01]  # Needed so it can be viewed from afar
#         create = Operation("create", Entity(name=self.gname,
#                                             type="construction",
#                                             bbox=bbox1,
#                                             location=chunk_loc),
#                            to=target)
#         create.set_serialno(0)
#         res.append(create)
#         res.append(self.next_tick(1.75))
#         return res
