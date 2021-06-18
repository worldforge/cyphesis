# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *

# class Trenching(server.Task):
#     """ A task for creating Trench with a pickaxe."""
#
#     materials = {0: 'boulder', 1: 'sand', 2: 'earth', 3: 'silt', 4: 'ice'}
#
#     def trench_operation(self, op):
#         """ Op handler for cut op which activates this task """
#         # print "Trenching.trench"
#
#         if len(op) < 1:
#             sys.stderr.write("Trenching task has no target in trench op")
#
#         # FIXME Use weak references, once we have them
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#         self.pos = Point3D(op[0].pos)
#
#     def tick_operation(self, op):
#         """ Op handler for regular tick op """
#         # print "Trenching.tick"
#
#         target = self.target()
#         if not target:
#             # print "Target is no more"
#             self.irrelevant()
#             return
#
#         # FIXME We are overriding the position specified above?
#         self.pos = self.character.location.pos
#
#         old_rate = self.rate
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
#
#         if not hasattr(self, 'terrain_mod'):
#             mods = target.terrain.find_mods(self.pos)
#             if len(mods) == 0:
#                 # There is no terrain mod where we are digging,
#                 # so we check if it is in the materials , and if so create
#                 # a trench
#                 surface = target.terrain.get_surface(self.pos)
#                 # print "SURFACE %d at %s" % (surface, self.pos)
#                 if surface not in Trenching.materials:
#                     # print "Not in material"
#                     self.irrelevant()
#                     return
#                 self.surface = surface
#
#                 y = self.character.location.pos.y - 1.0
#                 modmap = {
#                     'height': y,
#                     'shape': {
#                         'points': [[-1.0, -1.0],
#                                    [-1.0, 1.0],
#                                    [1.0, 1.0],
#                                    [1.0, -1.0]],
#                         'type': 'polygon'
#                     },
#                     'type': 'levelmod'
#                 }
#                 trenches_create = Operation("create",
#                                             Entity(name="trenches",
#                                                    type="path",
#                                                    location=chunk_loc,
#                                                    terrainmod=modmap),
#                                             to=target)
#                 res.append(trenches_create)
#             else:
#                 for mod in mods:
#                     if not hasattr(mod, 'name') or mod.name != 'trenches':
#                         # print "%s is no good" % mod.id
#                         continue
#                     # print "%s looks good" % mod.id
#                     mod.terrainmod.height -= 1.0
#                     # We have modified the attribute in place, so must send an update op to propagate
#                     res.append(Operation("update", to=mod.id))
#                     break
#             # self.terrain_mod = "moddy_mod_mod"
#
#         create = Operation("create",
#                            Entity(name=Trenching.materials[self.surface],
#                                   type="pile",
#                                   material=Trenching.materials[self.surface],
#                                   location=chunk_loc), to=target)
#         res.append(create)
#
#         res.append(self.next_tick(0.75))
#
#         return res
