# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

from atlas import *

import server
from world.utils import Usage

def strike(instance):
    # Check that we can reach the target with our weapon
    extraReach = 0.0
    if instance.tool.props.reach:
        extraReach = instance.tool.props.reach

    # If there's a cooldown we need to mark the actor
    Usage.set_cooldown_on_attached(instance.tool, instance.actor)

    # Send sight even if we miss
    instance.actor.send_world(Operation("sight", instance.op))

    # Sword only handles one target
    target = instance.targets[0]
    # Ignore pos
    if instance.actor.can_reach(target, extraReach):
        damage = 0
        if instance.tool.props.damage:
            damage = instance.tool.props.damage
        hitOp = Operation('hit', Entity(damage=damage, hit_type=instance.op.id), to=target.entity)
        return (server.OPERATION_BLOCKED, hitOp, Operation('sight', hitOp))
    else:
        return (server.OPERATION_BLOCKED, instance.actor.client_error(instance.op, "Too far away"))
