# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2006 Al Riddoch (See the file COPYING for details)
# Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *

# class Bisect(server.Task):
#     """ A task for cutting a section of material in two."""
#
#     def cut_operation(self, op):
#         """ Op handler for cut op which activates this task """
#         # print "Bisect.cut"
#
#         if len(op) < 1:
#             sys.stderr.write("Bisect task has no target in cut op")
#
#         # FIXME Use weak references, once we have them
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#     def tick_operation(self, op):
#         """ Op handler for regular tick op """
#         # print "Bisect.tick"
#
#         if self.target is None:
#             # print "Target is no more"
#             self.irrelevant()
#             return
#
#         if square_distance(self.character.location, self.target().location) > (self.target().location.bbox.square_bounding_radius() + 1):
#             self.rate = 0
#             # print "Too far away"
#             return self.next_tick(0.75)
#         else:
#             self.progress += 1
#
#         self.rate = 0.5 / 0.75
#
#         if self.progress < 1:
#             # print "Not done yet"
#             return self.next_tick(0.75)
#
#         # this is the axis defining the plane of the cut. Once tasks can
#         # be parameterized we can vary this to any axis
#         cut_plane = 2  # z
#
#         # work out half the materials size in this dimension. This should
#         # become a parameter too
#         length = self.target().location.bbox.high_corner[cut_plane] - \
#                  self.target().location.bbox.low_corner[cut_plane]
#         mid = length / 2
#
#         res = Oplist()
#         # A new BBox which changes the size of original entity
#         new_bbox = [self.target().location.bbox.low_corner.x,
#                     self.target().location.bbox.low_corner.y,
#                     self.target().location.bbox.low_corner.z,
#                     self.target().location.bbox.high_corner.x,
#                     self.target().location.bbox.high_corner.y,
#                     self.target().location.bbox.high_corner.z]
#
#         # reduce the size of the existing entities bbox by the length to be
#         # removed, along the correct axis
#         new_bbox[cut_plane + 3] -= mid
#
#         # print "mid ", mid
#
#         # print "mod",self.target().location.pos, new_bbox
#
#         set = Operation("set", Entity(self.target().id, bbox=new_bbox), to=self.target())
#         res.append(set)
#
#         # A new entity is to be created by this cut task, and we use the
#         # original entity for the basis of the updated location
#         slice_loc = self.target().location.copy()
#
#         # Determine the position offset in the targets coord system
#         # FIXME This will be different once we deal with cuts which are not
#         # perfect halves.
#         pos_offset = Vector3D(0, 0, 0)
#         pos_offset[cut_plane] = mid
#
#         # transform this into the world coords if necessary
#         if self.target().location.orientation.is_valid():
#             pos_offset.rotate(self.target().location.orientation)
#
#         # apply the offset to the new entity position
#         slice_loc.pos = self.target().location.pos + pos_offset
#
#         slice_bbox = [self.target().location.bbox.low_corner.x,
#                       self.target().location.bbox.low_corner.y,
#                       self.target().location.bbox.low_corner.z,
#                       self.target().location.bbox.high_corner.x,
#                       self.target().location.bbox.high_corner.y,
#                       self.target().location.bbox.high_corner.z]
#
#         # set the size of the bbox of this new entity to the know slice size
#         slice_bbox[cut_plane] = 0.0
#         slice_bbox[cut_plane + 3] = mid
#
#         # print "new",slice_loc.pos, slice_bbox
#
#         slice_loc.orientation = self.target().location.orientation
#
#         typ = str(self.target().type[0])
#         # Some entity do not have name attribute defined. If name not present assign the same name as type.
#         if hasattr(self.target(), 'name'):
#             nam = str(self.target().name)
#         else:
#             nam = typ
#
#         # create a new fragment of the remaining dimensions
#         create = Operation("create", Entity(name=nam,
#                                             type=typ,
#                                             location=slice_loc,
#                                             bbox=slice_bbox), to=self.target())
#         res.append(create)
#
#         self.progress = 1
#         self.irrelevant()
#
#         return res
