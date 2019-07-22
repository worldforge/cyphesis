# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

from atlas import Operation, Entity, Oplist
from physics import Vector3D
from random import *

import server


class Gravestone(server.Thing):
    """
    When digging at a grave, skeleton parts will be created.
    """
    items = ['femur', 'humerus', 'pelvis', 'ribcage', 'skull', 'tibia']

    def sift_operation(self, op):
        newloc = self.location.copy()
        newloc.velocity = Vector3D()
        item = Gravestone.items[randint(0, 5)]
        newloc.pos = newloc.pos + Vector3D(uniform(-1, 1), uniform(-1, 1), uniform(-1, 1))
        return Operation("create", Entity(name=item, parent=item, location=newloc.copy()), to=self) + \
               Operation("imaginary", Entity(description="You dig up a bone from the grave."), to=op.id, from_=op.id)
