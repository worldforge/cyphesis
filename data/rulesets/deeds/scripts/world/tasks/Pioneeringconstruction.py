# This file is distributed under the terms of the GNU General Public
# license. #Copyright (C) 2012 Anthony Pesce <timetopat@gmail.com> (See
# the file COPYING for details).

from atlas import *
from physics import *

# class Pioneeringconstruction(server.Task):
#     """A task for creating a Wooden structures such as
#     A Frames with lumber and rope"""
#
#     materials = "lumber"
#
#     def aframe_operation(self, op):
#         """ Op handler for Pioneeringconstruction op
#         which activates this task """
#
#         if len(op) < 1:
#             sys.stderr.write("Pioneeringconstruction  task has no target "
#                              " in op")
#
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#         self.pos = Point3D(op[0].pos)
#
#     def info_operation(self, op):
#         print("Aframe info")
#         aframe = server.world.get_entity(op[0].id)
#         self.lcount = 0
#
#         raw_materials = []
#         for item in self.character.contains:
#             if item.type[0] == str(self.materials):
#                 raw_materials.append(item)
#                 self.lcount = self.lcount + 1
#             if self.lcount == 3:
#                 break
#         else:
#             print("No materials in inventory for A frame")
#             self.irrelevant()
#             return
#
#         chunk_loc = Location(aframe())
#         chunk_loc.pos = Point3D([0, 0, 0])
#
#         count = self.lcount
#         res = Oplist()
#         # loops through raw_materials and places 3 lumber
#         # in inventory infront of user
#         offset = Vector3D(0, 0, 0)
#         while (count > 0):
#             tar = raw_materials.pop()
#             # length of the lumber obtained
#             lumberlength = tar.location.bbox.high_corner[2] - \
#                            tar.location.bbox.low_corner[2]
#             lumberheight = tar.location.bbox.high_corner[1] - \
#                            tar.location.bbox.low_corner[1]
#             # rough length to position lumber
#             lumber_length = lumberlength / 4
#
#             if count == 3:
#                 # left component
#                 chunk_loc.orientation = Quaternion([.653, 0.27, .27, .653])
#             if count == 2:
#                 # right component
#                 chunk_loc.orientation = Quaternion([.653, -0.27, -.27, .653])
#                 offset = Vector3D(lumber_length, 0, 0)
#                 chunk_loc.pos = chunk_loc.pos + offset
#             if count == 1:
#                 # bottom component
#                 chunk_loc.pos = Point3D([0, 0, 0])  # self.pos
#                 # .707 is sin(.5) which is needed for a 90 degree rotation
#                 chunk_loc.orientation = Quaternion([.707, 0, .707, 0])
#                 offset = Vector3D(-(1.5 * lumber_length), 0, (2.5 * lumber_length))
#                 chunk_loc.pos = chunk_loc.pos + offset
#
#             move = Operation("move", Entity(tar.id, location=chunk_loc,
#                                             mode="fixed"), to=tar)
#             res.append(move)
#             count = count - 1
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
#         if self.progress < 1:
#             # print "Not done yet"
#             return self.next_tick(1.75)
#
#         self.progress = 0
#
#         chunk_loc = Location(self.character.parent)
#         chunk_loc.pos = self.pos
#         lumberh = 0  # lumberheight
#         lumberl = 0  # lumberlength
#         res = Oplist()
#         lcount = 0
#         # makes sure we have 3 lumber to construct A frame
#         for item in self.character.contains:
#             if item.type[0] == str(self.materials):
#                 lcount = lcount + 1
#                 lumberl = item.location.bbox.high_corner[2] - \
#                           item.location.bbox.low_corner[2]
#                 lumberh = item.location.bbox.high_corner[1] - \
#                           item.location.bbox.low_corner[1]
#
#             if lcount == 3:
#                 break
#         else:
#             print("No materials in inventory for A frame")
#             self.irrelevant()
#             return
#
#         bbox1 = [-lumberl / 2, -lumberl / 2, -lumberh / 2, lumberl / 2, lumberl / 2, lumberh / 2]
#         # bbox of a frame
#         create = Operation("create", Entity(name="A_Frame",
#                                             type="construction",
#                                             bbox=bbox1, location=chunk_loc),
#                            to=target)
#         create.set_serialno(0)
#         res.append(create)
#         res.append(self.next_tick(1.75))
#         return res
