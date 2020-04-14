# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *

# class Trailblaze(server.Task):
#     """ A proof of concept task for making new paths and roads."""
#
#     def strike_operation(self, op):
#         """ Op handler for strike op which activates this task """
#         # print "Trailblaze.strike"
#
#         if len(op) < 1:
#             sys.stderr.write("Trailblaze task has no target in strike op")
#
#         # FIXME Use weak references, once we have them
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#         self.stuff = 'stuff'
#         self.things = 'things'
#         self.points = []
#
#     def tick_operation(self, op):
#         """ Op handler for regular tick op """
#         # print "Trailblaze.tick"
#         res = Oplist()
#
#         target = self.target()
#         if not target:
#             # print "Target is no more"
#             self.irrelevant()
#             return
#
#         if not self.points:
#             self.rate = 0
#             self.progress = 0
#             if 'world' in target.type:
#                 new_loc = Location(target, self.character.location.pos)
#                 create = Operation("create", Entity(name='pile', type='pile',
#                                                     location=new_loc), to=target)
#                 res.append(create)
#                 self.points.append(self.character.location.pos)
#             elif 'pile' in target.type:
#                 print('Pile')
#             else:
#                 print('WTF!', target.type)
#                 self.irrelevant()
#                 return
#         else:
#             if not self.character.location.velocity.is_valid() or \
#                 self.character.location.velocity.sqr_mag() < 1:
#                 if self.character.location.pos != self.points[-1:][0]:
#                     self.points.append(self.character.location.pos)
#                 if self.rate:
#                     # Finish up, and create the path
#                     self._create_path(target, res)
#                     self.irrelevant()
#                     return res
#                 else:
#                     self.progress = 0
#                     self.rate = 1 / 1.75
#             else:
#                 self.progress = 0
#                 self.rate = 0
#                 if self.character.location.pos != self.points[-1:][0]:
#                     self.points.append(self.character.location.pos)
#         res.append(self.next_tick(1.75))
#         return res
#
#     def _create_path(self, target, res):
#         line = []
#         # The left side of the path
#         area = []
#         # The right side of the path
#         area_tail = []
#         count = len(self.points)
#
#         minx = 0
#         miny = 0
#         minz = 0
#
#         maxx = 0
#         maxy = 0
#         maxz = 0
#
#         origin = self.points[0]
#         for i in range(count):
#             point = self.points[i]
#             local_point = point - origin
#             line.append([local_point.x, local_point.y, local_point.z])
#             if i == 0:
#                 # The first point on the path. Make it the start of both sides
#                 vtn = (self.points[i + 1] - point).unit_vector()
#                 area.append([local_point.x - 2 * vtn.z, local_point.z + 2 * vtn.x])
#                 area_tail.append([local_point.x + 2 * vtn.z, local_point.z - 2 * vtn.x])
#                 # area_tail.append([point.x, point.y])
#                 continue
#
#             minx = min(minx, local_point.x)
#             miny = min(miny, local_point.y)
#             minz = min(minz, local_point.z)
#
#             maxx = max(maxx, local_point.x)
#             maxy = max(maxy, local_point.y)
#             maxz = max(maxz, local_point.z)
#
#             if i == count - 1:
#                 # The end point of the path. Make it the end of left side.
#                 vfp = (point - self.points[i - 1]).unit_vector()
#                 area.append([local_point.x - 2 * vfp.z, local_point.z + 2 * vfp.x])
#                 area_tail.append([local_point.x + 2 * vfp.z, local_point.z - 2 * vfp.x])
#                 continue
#             # vector from previous
#             vfp = (point - self.points[i - 1]).unit_vector()
#             # vector to next
#             vtn = (self.points[i + 1] - point).unit_vector()
#
#             area.append([local_point.x - vfp.z - vtn.z, local_point.z + vfp.x + vtn.x])
#             area_tail.append([local_point.x + vfp.z + vtn.z, local_point.z - vfp.x - vtn.x])
#
#         # Reverse the right side of the path
#         area_tail.reverse()
#         # and append it to the left side to make an area boundary
#         area += area_tail
#
#         # FIXME Add bbox
#         new_loc = Location(target, self.points[0])
#         # create = Operation('create', Entity(name='path', type='path', location=new_loc, area=area, line=line), to=target)
#         bbox = [minx, miny, minz, maxx, maxy, maxz]
#         print("area %r box %r" % (area, bbox))
#         create = Operation('create',
#                            Entity(name='path', type='path', location=new_loc, bbox=bbox,
#                                   area={'shape': {'points': area, 'type': 'polygon'},
#                                         'layer': 7}, line=line), to=target)
#         res.append(create)
#         return
