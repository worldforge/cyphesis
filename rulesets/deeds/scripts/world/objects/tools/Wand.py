# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

from atlas import Operation, Entity, Oplist

from physics import Vector3D, Point3D, Quaternion

import server
from world.StoppableTask import StoppableTask
from world.utils import Usage
import entity_filter


def shoot_in_direction(direction, instance, res):
    Usage.set_cooldown_on_attached(instance.tool, instance.actor)

    direction.normalize()

    # Adjust the start position of the projectile, so it's outside of the actor, at mid height
    start_adjust = Vector3D(direction)
    start_adjust.y = 0
    start_adjust.normalize()
    start_adjust.y = instance.actor.location.bbox.high_corner.y * 0.8

    new_loc = instance.actor.location.copy()
    new_loc.pos += start_adjust

    new_loc.orientation = Quaternion(Vector3D(0, 0, 1), direction, Vector3D(1, 0, 0))
    mode_data = {"mode": "projectile", "$eid": instance.actor.id}

    res.append(Operation("create", Entity(parent="fireball", location=new_loc, velocity=direction * 60, mode="projectile", mode_data=mode_data, damage_explosion=instance.tool.props.damage), to=instance.tool.id))


def fireball(instance):
    res = Oplist()
    direction = instance.get_arg("direction", 0)

    shoot_in_direction(direction, instance, res)
    charges = instance.tool.get_prop_int("charges", 1)
    set_op = Operation("set", Entity(instance.tool.id, charges=charges - 1), to=instance.tool)
    res.append(set_op)

    return server.OPERATION_BLOCKED, res


