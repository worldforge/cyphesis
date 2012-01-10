#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Point3D
from physics import Vector3D

import math
import weakref

import server

class Heaping(server.Task):
    """ A task for laying down built up terrain with a digging implement."""
    class Obstructed(Exception):
        "An exception indicating this task is obstructed by an entity>"
        pass

    def heap_operation(self, op):
        """ Op handler for cut op which activates this task """
        # print "Heaping.cut"

        if len(op) < 1:
            sys.stderr.write("Heaping task has no target in cut op")

        self.target = server.world.get_object_ref(op[0].id)
        self.tool = op.to

        self.pos = Point3D(op[0].pos)

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Heaping.tick"

        if self.target() is None:
            # print "Target is no more"
            self.irrelevant()
            return


        old_rate = self.rate

        self.rate = 0.5 / 0.75
        self.progress += 0.5

        if old_rate < 0.01:
            self.progress = 0

        # print "%s" % self.pos

        if self.progress < 1:
            # print "Not done yet"
            return self.next_tick(0.75)

        self.progress = 0

        res=Oplist()

        if not hasattr(self, 'terrain_mod') or self.terrain_mod() is None:
            try:
                mod = self._find_mod('motte')
            except Obstructed:
                self.irrelevant()
                return
            if mod is None:
                # There is no terrain mod where we are digging,
                print "no existing mod"
                z=self.character.location.coordinates.z + 1.0
                modmap = {'height': z,
                          'shape': Polygon([[ -0.7, -0.7 ],
                                            [ -1.0, 0.0 ],
                                            [ -0.7, 0.7 ],
                                            [ 0.0, 1.0 ],
                                            [ 0.7, 0.7 ],
                                            [ 1.0, 0.0 ],
                                            [ 0.7, -0.7 ],
                                            [ 0.0, -1.0 ]]).as_data(),
                          'type': 'levelmod' }
                area_map = {'shape': modmap['shape'],
                            'layer': 7}

                mound_loc = Location(self.character.location.parent)
                mound_loc.velocity = Vector3D()
                mound_loc.coordinates = self.pos

                motte_create=Operation("create",
                                       Entity(name="motte",
                                              type="path",
                                              location = mound_loc,
                                              terrainmod = modmap,
                                              area = area_map),
                                       to=self.target())
                res.append(motte_create)
                res.append(self.next_tick(0.75))
                return res
            else:
                print "found existing mod"
                self.terrain_mod = weakref.ref(mod)

        mod = self.terrain_mod()
        area = mod.terrainmod.shape.area()
        factor = math.sqrt((area + 1) / area)
        mod.terrainmod.shape *= factor
        mod.terrainmod.height += 1 / area
        box = mod.terrainmod.shape.footprint()
        mod.bbox = [box.low_corner().x,
                    box.low_corner().y,
                    0,
                    box.high_corner().x,
                    box.high_corner().y,
                    mod.terrainmod.height]

        area_map = {'shape': mod.terrainmod.shape.as_data(),
                    'layer': 7}
        # FIXME This area is not getting broadcast
        mod.area = area_map
        # We have modified the attribute in place,
        # so must send an update op to propagate
        res.append(Operation("update", to=mod.id))
        res.append(self.next_tick(0.75))
        return res
    def _find_mod(self, name):
        mods = self.target().terrain.find_mods(self.pos)
        if len(mods) != 0:
            for mod in mods:
                if hasattr(mod, 'name') and mod.name == 'motte':
                    return mod
            raise Obstructed, "Another mod is in the way"
