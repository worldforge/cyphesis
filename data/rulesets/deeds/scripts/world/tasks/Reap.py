# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *

#
# class Reap(server.Task):
#     """ A task for reaping grass from the ground."""
#
#     def cut_operation(self, op):
#         """ Op handler for cut op which activates this task """
#         # print "Reap.cut"
#
#         if len(op) < 1:
#             sys.stderr.write("Reap task has no target in cut op")
#
#         # FIXME Use weak references, once we have them
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#     def tick_operation(self, op):
#         """ Op handler for regular tick op """
#         # print "Reap.tick"
#
#         if self.target() is None:
#             # print "Target is no more"
#             self.irrelevant()
#             return
#
#         if self.count == 0:
#             self.count = int(self.target().props.mass)
#             # print "setting target mass to ", self.count
#
#         if not self.character.location.velocity.is_valid() or \
#             self.character.location.velocity.sqr_mag() < 1 or \
#             self.character.location.velocity.sqr_mag() > 10:
#             self.rate = 0
#             self.progress = 0
#             # print "Not moving the right speed"
#             return self.next_tick(1.75)
#
#         old_rate = self.rate
#
#         self.rate = 1.0 / 1.75
#         self.progress = 0.01
#
#         if old_rate < 0.1:
#             # print "Wasn't moving right speed"
#             return self.next_tick(1.75)
#
#         surface = self.target().props.terrain.get_surface(self.character.location.pos)
#         if surface is not 2:
#             # print "Not grass"
#             return self.next_tick(1.75)
#
#         res = Oplist()
#
#         chunk_loc = Location(self.character.parent)
#         chunk_loc.velocity = Vector3D()
#
#         chunk_loc.pos = self.character.location.pos
#
#         create = Operation("create",
#                            Entity(name="grass",
#                                   type="grass",
#                                   location=chunk_loc), to=self.target())
#         res.append(create)
#
#         res.append(self.next_tick(1.75))
#
#         return res
