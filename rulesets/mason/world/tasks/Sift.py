#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2009 Amey Parulekar (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Point3D
from physics import Vector3D

import math
import server

class Sift(server.Task):
    """A task for sifting through a pile of earth for earthworms"""
    
    materials = ['earth']
    def get_quality(self, location, target, moisture):
        z = location.z
        zval = math.exp(-z*z/2)
        if not hasattr(target, 'location'):
            return 0
        normal = target.location.parent.terrain.get_normal(location.x, location.y);
        print normal
        i = Vector3D(1, 0, 0)
        slope = normal.dot(i) / normal.mag()
        return zval + moisture + (1 - slope)

    def cut_operation(self, op):
        """ Op handler for cut op which activates this task """

        if len(op) < 1:
            sys.stderr.write("Sift task has no target in cut op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

        self.pos = Point3D(op[0].pos)

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Dig.tick"
        target = server.world.get_object(self.target)
        world = target.location.parent
        if not target:
            print "Target is no more"
            self.irrelevant()
            return

        material = target.name
        #print material
                
        if material not in Sift.materials:
            print "Not right material for earthworms"
            self.irrelevant()
            return

        old_rate = self.rate

        self.rate = 0.1 / 1.75
        self.progress += 0.1

        if old_rate < 0.01:
            self.progress = 0
        else:
            self.progress += 0.1

        # print "%s" % self.pos

        if self.progress < 1:
            # print "Not done yet"
            return self.next_tick(1.75)

        self.progress = 0

        res=Oplist()

        self_loc = Location(self.character)
        self_loc.velocity = Vector3D()
        if hasattr(world, 'moisture'):
            moisture = 10 * world.moisture
        else:
            moisture = 1
        self_loc.coordinates = self.pos

        quality = int(self.get_quality(self_loc.coordinates, target, moisture))
        print quality
        for i in range(int(quality/2), quality):
            res = res + Operation("create", Entity(name = "scrawny earthworm", parents = ["annelid"], location = self_loc), to=self.character)
        for i in range(int((10-quality)/2), quality):
            res = res + Operation("create", Entity(name = "juicy earthworm", parents = ["annelid"], location = self_loc), to=self.character)

        #res.append(self.next_tick(1.75))

        return res
