# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *

#
# class Heaping(server.Task):
#     """ A task for laying down built up terrain with a digging implement."""
#
#     class Obstructed(Exception):
#         "An exception indicating this task is obstructed by an entity>"
#         pass
#
#     def heap_operation(self, op):
#         """ Op handler for cut op which activates this task """
#         # print self.__class__.__name__,".setup"
#
#         if len(op) < 1:
#             sys.stderr.write("%s task has no target in setup op" % self.__class__.__name__)
#
#         self.target = server.world.get_entity(op[0].id)
#         self.tool = op.to
#
#         self.pos = Point3D(op[0].pos)
#
#     def tick_operation(self, op):
#         """ Op handler for regular tick op """
#         # print self.__class__.__name__,".tick"
#
#         if self.target() is None:
#             # print "Target is no more"
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
#         if not hasattr(self, 'terrain_mod') or self.terrain_mod() is None:
#             try:
#                 mod = self._find_mod('motte')
#             except Heaping.Obstructed:
#                 print("obstructed")
#                 self.irrelevant()
#                 return
#             if mod is None:
#                 # There is no terrain mod where we are digging,
#                 return self._create_initial_mod()
#             print("found existing mod")
#             self.terrain_mod = weakref.ref(mod)
#
#         return self._grow_mod()
#
#     def _grow_mod(self):
#         mod = self.terrain_mod()
#
#         # FIXME The face direction should be relative to the shape centre,
#         # rather than entity coords, then when the character is facing out,
#         # the shape should only grow in that direction
#
#         # Determine which way the user is facing relative to the mod
#         user_to_mod = (mod.location - self.character.location).unit_vector()
#         user_facing = Vector3D(1, 0, 0)
#         face_factor = user_facing.rotate(self.character.location.orientation)
#
#         area = mod.terrainmod.shape.area()
#         if face_factor > 0.2:
#             # Character is facing the centre. Heap up
#             mod.terrainmod.height += 1 / area
#         else:
#             # Character is facing away, Heap out
#             area_factor = math.sqrt((area + 1) / area)
#             mod.terrainmod.shape *= area_factor
#             area_map = {'shape': mod.terrainmod.shape.as_data(),
#                         'layer': 7}
#             # FIXME This area is not getting broadcast
#             mod.area = area_map
#
#         box = mod.terrainmod.shape.footprint()
#         mod.bbox = [box.low_corner().x,
#                     0,
#                     box.low_corner().z,
#                     box.high_corner().x,
#                     mod.terrainmod.height,
#                     box.high_corner().z]
#
#         # We have modified the attribute in place,
#         # so must send an update op to propagate
#         res = Oplist()
#         res.append(Operation("update", to=mod.id))
#         res.append(self.next_tick(0.75))
#         return res
#
#     def _find_mod(self, name):
#         mods = self.target().props.terrain.find_mods(self.pos)
#         if len(mods) != 0:
#             for mod in mods:
#                 if hasattr(mod, 'name') and mod.name == name:
#                     return mod
#             raise Heaping.Obstructed("Another mod is in the way")
#
#     def _create_initial_mod(self):
#         print("no existing mod")
#         y = self.character.location.pos.y + 1.0
#         modmap = {'height': y,
#                   'shape': Polygon([[-0.7, -0.7],
#                                     [-1.0, 0.0],
#                                     [-0.7, 0.7],
#                                     [0.0, 1.0],
#                                     [0.7, 0.7],
#                                     [1.0, 0.0],
#                                     [0.7, -0.7],
#                                     [0.0, -1.0]]).as_data(),
#                   'type': 'levelmod'}
#         area_map = {'shape': modmap['shape'],
#                     'layer': 7}
#
#         mod_loc = Location(self.character.parent)
#         mod_loc.velocity = Vector3D()
#         mod_loc.pos = self.pos
#
#         mod_create = Operation("create",
#                                Entity(name="motte",
#                                       type="path",
#                                       location=mod_loc,
#                                       terrainmod=modmap,
#                                       area=area_map),
#                                to=self.target())
#         res = Oplist()
#         res.append(mod_create)
#         res.append(self.next_tick(0.75))
#         return res
