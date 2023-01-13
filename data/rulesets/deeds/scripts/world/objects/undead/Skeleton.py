# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

import server
from atlas import Operation, Entity, Oplist
from physics import Vector3D
from rules import extract_location
from random import uniform

class Skeleton(server.Thing):
    def touch_operation(self, op):
        retops = Oplist()
        if self.props.status and self.props.status < 0:
            return
        newloc = extract_location(self)
        newloc.velocity = Vector3D()
        # retops += Operation("move", Entity(self.id, location=newloc.copy(), mode="collapsed"), to=self)
        for item in ['skull', 'ribcage', 'femur', 'pelvis', 'tibia']:
            newloc.pos = newloc.pos + Vector3D(uniform(-1, 1), uniform(-1, 1), uniform(-1, 1))
            retops += Operation("create", newloc.add_to_entity(Entity(name=item, parent=item)), to=self)
        retops += Operation("set", Entity(self.id, status=-1), to=self)
        return server.OPERATION_BLOCKED, retops
