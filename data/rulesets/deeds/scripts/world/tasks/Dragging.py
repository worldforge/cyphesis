# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *

#
# class Dragging(server.Task):
#     """ A task for dragging heavy objects by using a pulley ."""
#
#     def drag_operation(self, op):
#         """ Op handler for cut op which activates this task """
#         # print "Dragging.drag"
#
#         if len(op) < 1:
#             sys.stderr.write("Dragging task has no target in drag op")
#
#         # FIXME Use weak references, once we have them
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#         self.pos = Point3D(op[0].pos)
#
#     def tick_operation(self, op):
#         """ Op handler for regular tick op """
#         # print "Dragging.tick"
#         self.pos = self.character.location.pos
#         if self.target() is None:
#             # print "Target is no more"
#             self.irrelevant()
#             return
#
#         if not self.target().parent:
#             # Make sure the user dosen't use dragging on the world entity..
#             self.irrelevant()
#             return
#
#         old_rate = self.rate
#
#         self.rate = 0.5 / 0.75
#         self.progress += 0.5
#
#         if old_rate < 0.01:
#             self.progress = 0
#
#         # print "%s" % self.pos
#
#         if self.progress < 1:
#             # print "Not done yet"
#             return self.next_tick(0.75)
#
#         self.progress = 0
#
#         res = Oplist()
#
#         chunk_loc = Location(self.character.parent)
#         chunk_loc.velocity = Vector3D()
#
#         chunk_loc.pos = self.pos
#         # Move the entity to user's position.
#         res = res + Operation("move", Entity(self.target().id,
#                                              location=chunk_loc), to=self.target())
#         res.append(self.next_tick(0.75))
#
#         return res
