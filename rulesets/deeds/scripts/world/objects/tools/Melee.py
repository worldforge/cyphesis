# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

from atlas import Operation, Entity

import server
from world.utils import Usage


def strike(instance):
    """Strike another entity with a weapon."""

    # Check that we can reach the target with our weapon
    extra_reach = 0.0
    if instance.tool.props.reach:
        extra_reach = instance.tool.props.reach

    # If there's a cooldown we need to mark the actor
    Usage.set_cooldown_on_attached(instance.tool, instance.actor)

    # Send sight even if we miss
    instance.actor.send_world(Operation("sight", instance.op))

    # Melee weapons only handles one target
    target = instance.get_arg("targets", 0)
    if target:
        # Ignore pos
        if instance.actor.can_reach(target, extra_reach):
            damage = 0
            if instance.tool.props.damage:
                damage = instance.tool.props.damage
            print(instance.actor.id)
            hit_op = Operation('hit', Entity(damage=damage, hit_type=instance.op.parent), to=target.entity, id=instance.actor.id)
            return server.OPERATION_BLOCKED, hit_op, Operation('sight', hit_op)
        else:
            return server.OPERATION_BLOCKED, instance.actor.client_error(instance.op, "Too far away")
    else:
        return server.OPERATION_BLOCKED
