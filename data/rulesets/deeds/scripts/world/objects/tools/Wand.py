# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

import server
from atlas import Operation, Entity, Oplist
from physics import Vector3D, Quaternion

from world.utils import Usage


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

    # TODO: match with animation in client
    res.append(instance.actor.start_action("wand/releasing", 1))
    res.append(Operation("create",
                         Entity(parent="fireball", location=new_loc, velocity=direction * 60, mode="projectile",
                                mode_data=mode_data, damage_explosion=instance.tool.props.damage),
                         to=instance.tool.id))


def fireball(instance):
    res = Oplist()
    direction = instance.get_arg("direction", 0)

    shoot_in_direction(direction, instance, res)
    set_op = Operation("set", Entity(instance.tool.id, {"charges!subtract": 1}), to=instance.tool)
    res.append(set_op)

    return server.OPERATION_BLOCKED, res


def shoot_poison_in_direction(direction, instance, res):
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

    # TODO: match with animation in client
    res.append(instance.actor.start_action("wand/releasing", 1))
    res.append(Operation("create",
                         Entity(parent="poisonball", location=new_loc, velocity=direction * 60, mode="projectile",
                                mode_data=mode_data, damage_poison=instance.tool.props.damage),
                         to=instance.tool.id))


def poison(instance):
    res = Oplist()
    direction = instance.get_arg("direction", 0)

    shoot_poison_in_direction(direction, instance, res)
    set_op = Operation("set", Entity(instance.tool.id, {"charges!subtract": 1}), to=instance.tool)
    res.append(set_op)

    return server.OPERATION_BLOCKED, res
