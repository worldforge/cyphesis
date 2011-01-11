#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Point3D
from physics import Vector3D

import server

class Heaping(server.Task):
    """ A task for laying down built up terrain with a digging implement."""

    def heap_operation(self, op):
        """ Op handler for cut op which activates this task """
        # print "Heaping.cut"

        if len(op) < 1:
            sys.stderr.write("Heaping task has no target in cut op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

        self.pos = Point3D(op[0].pos)

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Heaping.tick"

        target=server.world.get_object(self.target)
        if not target:
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

        chunk_loc = Location(self.character.location.parent)
        chunk_loc.velocity = Vector3D()

        chunk_loc.coordinates = self.pos

        if not hasattr(self, 'terrain_mod'):
            mods = target.terrain.find_mods(self.pos)
            if len(mods) == 0:
                # There is no terrain mod where we are digging,
                z=self.character.location.coordinates.z + 1.0
                print 'Initial z', z
                motte_create=Operation("create",
                                       Entity(name="motte",
                                              type="path",
                                              location = chunk_loc,
                                              terrainmod = {
                                                            'height': z,
                                                            'shape': {
                                                                      'points': [[ -1.0, -1.0 ],
                                                                                 [ -1.0, 1.0 ],
                                                                                 [ 1.0, 1.0 ],
                                                                                 [ 1.0, -1.0 ]],
                                                                      'type': 'polygon'
                                                                      },
                                                            'type': 'levelmod'
                                                           }),
                                       to=target)
                res.append(motte_create)
            else:
                print mods
                for mod in mods:
                    if not hasattr(mod, 'name') or mod.name != 'motte':
                        print "%s is no good" % mod.id
                        continue
                    print "%s(%s) looks good" % (mod.name, mod.id)
                    print mod.terrainmod
                    print mod.terrainmod.height
                    mod.terrainmod.height += 1.0
                    print mod.terrainmod.height
                    # We have modified the attribute in place,
                    # so must send an update op to propagate
                    res.append(Operation("update", to=mod.id))
                    break
            # self.terrain_mod = "moddy_mod_mod"



        res.append(self.next_tick(0.75))

        return res
