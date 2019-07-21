# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2005 Erik Hjortsberg (See the file COPYING for details).

from atlas import Operation, Entity, Oplist
from physics import Vector3D
from random import *

import server


class Gravestone(server.Thing):
    """
    When digging at a grave, skeleton parts will be created.
    """

    def create_skeletonpart(self):
        retops = Oplist()
        newloc = self.location.copy()
        newloc.velocity = Vector3D()
        items = ['femur', 'humerus', 'pelvis', 'ribcage', 'skull', 'tibia']
        item = items[randint(0, 5)]
        newloc.pos = newloc.pos + Vector3D(uniform(-1, 1), uniform(-1, 1), uniform(-1, 1))
        retops += Operation("create", Entity(name=item, parent=item, location=newloc.copy()), to=self)
        return retops

    def dig_operation(self, op):
        return self.create_skeletonpart()
