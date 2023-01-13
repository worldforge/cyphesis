# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

from random import randint

import server
from atlas import Operation, Entity
from physics import Vector3D


class Gravestone(server.Thing):
    """
    When digging at a grave, skeleton parts will be created.
    """
    items = ['femur', 'humerus', 'pelvis', 'ribcage', 'skull', 'tibia']

    def sift_operation(self, op):
        newloc = self.location.copy()
        newloc.velocity = Vector3D()
        item = self.items[randint(0, len(self.items) - 1)]
        newloc.pos = newloc.pos + Vector3D(uniform(-1, 1), uniform(-1, 1), uniform(-1, 1))
        return Operation("create", Entity(name=item, parent=item, location=newloc.copy()), to=self) + \
               Operation("imaginary", Entity(description="You dig up some bones from the grave."), to=op.id, from_=op.id)
