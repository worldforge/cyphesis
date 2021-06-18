# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2012 Anthony Pesce <timetopat@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *

#
# class Furnishings(server.Task):
#     """A task for creating a Wooden structures such as Chairs,and Tables with a hammer"""
#
#     materials = "lumber"
#
#     def furnishing_operation(self, op):
#         """ Op handler for Furnishings op which activates this task """
#
#         if len(op) < 1:
#             sys.stderr.write("Furnishings  task has no target in op")
#
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#         self.pos = Point3D(op[0].pos)
#
#     def info_operation(self, op):
#         print("Furnishings info")
#         item = server.world.get_entity(op[0].id)
#         chunk_loc = Location(item())
#         chunk_loc.pos = Point3D([0, 0, 0])
#         res = Oplist()
#         target = self.target()
#         raw_materials = []
#         raw_materials1 = []  # holders campfire
#         raw_materials2 = []  # temp holder for aframes to be moved
#         acount = 0  # A frame count
#         lcount = 0  # Lumber count
#         wcount = 0  # Wood count
#         bcount = 0  # Boulder count
#         ccount = 0  # Campfire count
#         for item in self.character.contains:
#             if item.type[0] == "lumber":
#                 raw_materials.append(item)
#                 lcount = lcount + 1
#             if item.type[0] == "wood":
#                 raw_materials.append(item)
#                 wcount = wcount + 1
#             if item.type[0] == "campfire":
#                 raw_materials1.append(item)
#                 ccount = ccount + 1
#             if item.type[0] == "boulder":
#                 raw_materials.append(item)
#                 bcount = bcount + 1
#             if item.type[0] == "construction":
#                 raw_materials2.append(item)
#                 acount = acount + 1
#
#         count = lcount + wcount + bcount
#         if self.fname == "Table":
#             # Making table
#             while (count > 0):
#                 tar = raw_materials.pop()
#                 set = Operation("set", Entity(tar.id, status=-1), to=tar)
#                 res.append(set)
#                 count = count - 1
#             # create the table
#             # Table base
#             lbbox = [-.2, -.2, -.5, .2, .2, .5]  # local bbox
#             create = Operation("create", Entity(name="lumber", type="lumber", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
#             # create the table top
#             offset = Vector3D(0, 1, 0)
#             chunk_loc.pos = chunk_loc.pos + offset
#             lbbox = [-.7, -.7, -.1, .7, .7, .1]
#             create = Operation("create", Entity(name="wood", type="wood", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
#         if self.fname == "Chair":
#             # Making chair
#             while (count > 0):
#                 tar = raw_materials.pop()
#                 set = Operation("set", Entity(tar.id, status=-1), to=tar)
#                 res.append(set)
#                 count = count - 1
#             # create the legs
#             # leg 1
#             chunk_loc.pos = Point3D([0, 0, 0])
#             offset = Vector3D(.2, 0, -.2)
#             chunk_loc.pos = chunk_loc.pos + offset
#             create = Operation("create", Entity(name="lumber", type="lumber", location=chunk_loc, mode="fixed"), to=target)
#             res.append(create)
#
#             # leg 2
#             chunk_loc.pos = Point3D([0, 0, 0])
#             offset = Vector3D(-.2, 0, -.2)
#             chunk_loc.pos = chunk_loc.pos + offset
#             create = Operation("create", Entity(name="lumber", type="lumber", location=chunk_loc, mode="fixed"), to=target)
#             res.append(create)
#
#             # leg 3
#             chunk_loc.pos = Point3D([0, 0, 0])
#             offset = Vector3D(-.2, 0, .2)
#             chunk_loc.pos = chunk_loc.pos + offset
#             create = Operation("create", Entity(name="lumber", type="lumber", location=chunk_loc, mode="fixed"), to=target)
#             res.append(create)
#
#             # leg 4
#             chunk_loc.pos = Point3D([0, 0, 0])
#             offset = Vector3D(.2, 0, .2)
#             chunk_loc.pos = chunk_loc.pos + offset
#             create = Operation("create", Entity(name="lumber", type="lumber", location=chunk_loc, mode="fixed"), to=target)
#             res.append(create)
#
#             # create the seat
#             chunk_loc.pos = Point3D([0, 0, 0])
#             offset = Vector3D(0, .5, 0)
#             lbbox = [-.3, -.3, -.1, .3, .3, .1]  # Local bbox
#             chunk_loc.pos = chunk_loc.pos + offset
#             create = Operation("create", Entity(name="wood", type="wood", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
#
#             # create the back of the seat
#             chunk_loc.pos = Point3D([0, 0, 0])
#             offset = Vector3D(-.3, .75, 0)
#             lbbox = [-.1, -.3, -.4, .1, .3, .4]  # local bbox
#             chunk_loc.pos = chunk_loc.pos + offset
#             create = Operation("create", Entity(name="wood", type="wood", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
#
#         if self.fname == "Floor":
#             # Making Floor
#             while (count > 0):
#                 tar = raw_materials.pop()
#                 set = Operation("set", Entity(tar.id, status=-1), to=tar)
#                 res.append(set)
#                 count = count - 1
#             # create the Floor, it is one large wood
#             lbbox = [-2, -2, -.1, 2, 2, .1]  # local bbox
#             create = Operation("create", Entity(name="wood", type="wood", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
#         if self.fname == "Siding":
#             # Making wooden siding with window
#             while (count > 0):
#                 tar = raw_materials.pop()
#                 set = Operation("set", Entity(tar.id, status=-1), to=tar)
#                 res.append(set)
#                 count = count - 1
#             # Siding is made of 4 components so it looks like we have a window
#             # Bottom part
#             lbbox = [-3, -.1, -1, 3, .1, 2]
#             create = Operation("create", Entity(name="wood", type="wood", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
#             # Top part
#             chunk_loc.pos = Point3D([0, 0, 0])
#             offset = Vector3D(0, 4, 0)
#             chunk_loc.pos = chunk_loc.pos + offset
#             lbbox = [-3, -.1, -1, 3, .1, 2]
#             create = Operation("create", Entity(name="wood", type="wood", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
#             # left part
#             chunk_loc.pos = Point3D([0, 0, 0])
#             offset = Vector3D(-2, 2, 0)
#             chunk_loc.pos = chunk_loc.pos + offset
#             lbbox = [-1, -.1, -.5, 1, .1, .5]
#             create = Operation("create", Entity(name="wood", type="wood", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
#             # Right part
#             chunk_loc.pos = Point3D([0, 0, 0])
#             offset = Vector3D(2, 2, 0)
#             chunk_loc.pos = chunk_loc.pos + offset
#             lbbox = [-1, -.1, -.5, 1, .1, .5]
#             create = Operation("create", Entity(name="wood", type="wood", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
#         if self.fname == "Fireplace":
#             # Making Fireplace
#             while (count > 0):
#                 tar = raw_materials.pop()
#                 set = Operation("set", Entity(tar.id, status=-1), to=tar)
#                 res.append(set)
#                 count = count - 1
#             # Move campfire
#             tar = raw_materials1.pop()
#             create = Operation("move", Entity(tar.id, location=chunk_loc, mode="fixed"), to=tar)
#             res.append(create)
#             # make floor of fireplace
#             lbbox = [-2, -1, -.1, 2, 1, .1]  # local bbox
#             create = Operation("create", Entity(name="boulder", type="boulder", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
#             # make wall 1 of fireplace
#             chunk_loc.pos = Point3D([0, 0, 0])
#             offset = Vector3D(-1.6, 0, 0)
#             lbbox = [-.1, -1, -.1, .1, 1, 1.5]
#             chunk_loc.pos = chunk_loc.pos + offset
#             create = Operation("create", Entity(name="boulder", type="boulder", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
#             # make wall 2 of fireplace
#             chunk_loc.pos = Point3D([0, 0, 0])
#             offset = Vector3D(1.6, 0, 0)
#             lbbox = [-.1, -1, -.1, .1, 1, 1.5]
#             chunk_loc.pos = chunk_loc.pos + offset
#             create = Operation("create", Entity(name="boulder", type="boulder", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
#             # make back of fireplace
#             chunk_loc.pos = Point3D([0, 0, 0])
#             offset = Vector3D(0, 0, .6)
#             lbbox = [-2, -.1, -.1, 2, .1, 1.5]
#             chunk_loc.pos = chunk_loc.pos + offset
#             create = Operation("create", Entity(name="boulder", type="boulder", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
#         if self.fname == "Wallframe":
#             # Bottom part of wall frame
#             chunk_loc.pos = Point3D([0, 0, 0])
#             tar = raw_materials.pop()
#             lumberlength = tar.location.bbox.high_corner[2] - tar.location.bbox.low_corner[2]
#             chunk_loc.orientation = Quaternion([.5, .5, .5, .5])
#             offset = Vector3D(-lumberlength / 2, 0, 0)
#             chunk_loc.pos = chunk_loc.pos + offset
#             create = Operation("move", Entity(tar.id, location=chunk_loc, mode="fixed"), to=tar)
#             res.append(create)
#             # Top part of wall frame
#             chunk_loc.pos = Point3D([0, 0, 0])
#             tar = raw_materials.pop()
#             offset = Vector3D(-lumberlength / 2, 0, lumberlength * .8)
#             chunk_loc.pos = chunk_loc.pos + offset
#             chunk_loc.orientation = Quaternion([.5, .5, .5, .5])
#             create = Operation("move", Entity(tar.id, location=chunk_loc, mode="fixed"), to=tar)
#             res.append(create)
#             # Left part of wall frame
#             chunk_loc.pos = Point3D([0, 0, 0])
#             tar = raw_materials.pop()
#             offset = Vector3D((lumberlength / 2.0) * -.8, lumberlength * .8, 0)
#             chunk_loc.pos = chunk_loc.pos + offset
#             chunk_loc.orientation = Quaternion([.707, .707, 0, 0])
#             create = Operation("move", Entity(tar.id, location=chunk_loc, mode="fixed"), to=tar)
#             res.append(create)
#             # Right part of wall frame
#             chunk_loc.pos = Point3D([0, 0, 0])
#             tar = raw_materials.pop()
#             offset = Vector3D((lumberlength / 2.0) * .8, lumberlength * .8, 0)
#             chunk_loc.pos = chunk_loc.pos + offset
#             chunk_loc.orientation = Quaternion([.707, .707, 0, 0])
#             create = Operation("move", Entity(tar.id, location=chunk_loc, mode="fixed"), to=tar)
#             res.append(create)
#             # Center part of wall frame
#             chunk_loc.pos = Point3D([0, 0, 0])
#             tar = raw_materials.pop()
#             offset = Vector3D((lumberlength / 2.0) * -.8, lumberlength * .1, 0)
#             chunk_loc.pos = chunk_loc.pos + offset
#             chunk_loc.orientation = Quaternion([.27, .27, .65, .65])
#             create = Operation("move", Entity(tar.id, location=chunk_loc, mode="fixed"), to=tar)
#             res.append(create)
#         if self.fname == "Roof":
#             # Making the roof, roof is 1 lumber and a large wooden covering
#             while (count > 0):
#                 tar = raw_materials.pop()
#                 set = Operation("set", Entity(tar.id, status=-1), to=tar)
#                 res.append(set)
#                 count = count - 1
#             # create the top, it is one large wood
#             lbbox = [-4, -4, -.1, 4, 4, .3]  # local bbox
#             offset = Vector3D(0, 5, 0)
#             chunk_loc.pos = chunk_loc.pos + offset
#             create = Operation("create", Entity(name="wood", type="wood", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
#             # create column
#             chunk_loc.pos = Point3D([0, 0, 0])
#             lbbox = [-.5, -.5, -.1, .5, .5, 6]  # local bbox
#             offset = Vector3D(0, 0, 0)
#             chunk_loc.pos = chunk_loc.pos + offset
#             create = Operation("create", Entity(name="lumber", type="lumber", location=chunk_loc, bbox=lbbox, mode="fixed"), to=target)
#             res.append(create)
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
#             return self.next_tick(0.75)
#
#         self.progress = 0
#
#         chunk_loc = Location(self.character.parent)
#         chunk_loc.pos = self.pos
#
#         chunk_loc.orientation = self.character.location.orientation
#         res = Oplist()
#         acount = 0  # A frame count
#         lcount = 0  # lumber count
#         wcount = 0  # wood count
#         ccount = 0  # campfire count
#         bcount = 0  # boulder count
#         self.fname = ""  # furnishing name
#         lumberwidth = 0
#         # makes sure we have the right amount of material
#         for item in self.character.contains:
#             if item.type[0] == "lumber":
#                 lcount = lcount + 1
#                 lumberwidth = item.location.bbox.high_corner[2] - item.location.bbox.low_corner[2]
#             if item.type[0] == "wood":
#                 wcount = wcount + 1
#             if item.type[0] == "campfire":
#                 ccount = ccount + 1
#             if item.type[0] == "boulder":
#                 bcount = bcount + 1
#             if item.type[0] == "construction":
#                 acount = acount + 1
#
#         print(str(lcount))
#         print(str(wcount))
#         if lcount == 1 and wcount == 3:
#             self.fname = "Table"
#         elif lcount == 4 and wcount == 2:
#             self.fname = "Chair"
#         elif lcount == 0 and wcount == 5:
#             self.fname = "Floor"
#         elif lcount == 0 and wcount == 4:
#             self.fname = "Siding"
#         elif ccount == 1 and bcount == 4:
#             self.fname = "Fireplace"
#         elif lcount == 3 and wcount == 5:
#             self.fname = "Roof"
#         elif wcount == 0 and lcount == 5:
#             self.fname = "Wallframe"
#         else:
#             print("No materials in inventory for Furnishings 1")
#             self.irrelevant()
#             return
#
#         bbox1 = [-1, -1, -1, 1, 1, 1]  # cube bbox so the ojects can be viewed from afar.  Relatively close fit
#         if (self.fname == "Floor"):  # If floor make different bbox which is thing so it can be walked over
#             bbox1 = [-2, -2, -.01, 2, 2, .01]
#         if (self.fname == "Wallframe"):  # If wall frame make bbox based upon the 2 aframes used
#             bbox1 = [-lumberwidth, -.5, -lumberwidth, 0, .5, lumberwidth]
#         if (self.fname == "Siding"):
#             bbox1 = [-3, -.1, -3, 3, .1, 3]
#         create = Operation("create", Entity(name=self.fname, type="construction", bbox=bbox1, location=chunk_loc), to=target)
#         create.set_serialno(0)
#         res.append(create)
#         res.append(self.next_tick(1.75))
#         return res
