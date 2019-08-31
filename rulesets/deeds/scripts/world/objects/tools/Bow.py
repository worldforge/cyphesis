# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

from atlas import Operation, Entity, Oplist

from physics import Vector3D, Point3D, Quaternion

import server
from world.utils import Usage
import entity_filter

arrow_filter = entity_filter.Filter("entity instance_of types.arrow")


def shoot(instance):
    Usage.set_cooldown_on_attached(instance.tool, instance.actor)

    res = Oplist()

    arrows = instance.actor.find_in_contains(arrow_filter)
    if len(arrows):
        direction = instance.get_arg("direction", 0)
        direction.normalize()

        # Adjust the start position of the arrow, so it's outside of the actor, at mid height
        start_adjust = Vector3D(direction)
        start_adjust.y = 0
        start_adjust.normalize()
        start_adjust.y = instance.actor.location.bbox.high_corner.y * 0.8

        new_loc = instance.actor.location.copy()
        new_loc.pos += start_adjust

        new_loc.orientation = Quaternion(Vector3D(0, 0, 1), direction, Vector3D(1, 0, 0))

        res.append(Operation("move", Entity(arrows[0].id, location=new_loc, velocity=direction * 60, mode="projectile"), to=arrows[0].id))
    return server.OPERATION_BLOCKED, res
