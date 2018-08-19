# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

from atlas import *

import server
from world.utils import Usage


def set_cooldown(actor):
    cooldown = actor.cooldown_punch
    if cooldown and cooldown > 0.0:
        ready_at_attached_prop = actor.props._ready_at_attached
        if not ready_at_attached_prop:
            ready_at_attached_prop = {}

        ready_at_attached_prop['attached_hand_primary'] = server.world.get_time() + cooldown
        actor.send_world(Operation('set', Entity(actor.id, _ready_at_attached=ready_at_attached_prop), to=actor.id))


def punch(instance):
    # If there's a cooldown we need to mark the actor
    set_cooldown(instance.actor)

    # Send sight even if we miss
    instance.actor.send_world(Operation("sight", instance.op))

    # Punching only handles one target
    target = instance.targets[0]
    # Ignore pos
    if instance.actor.can_reach(target):
        damage = 0
        if instance.actor.props.damage_punch:
            damage = instance.actor.props.damage_punch
        hitOp = Operation('hit', Entity(damage=damage, hit_type=instance.op.id), to=target.entity)
        return (server.OPERATION_BLOCKED, hitOp, Operation('sight', hitOp))
    else:
        return (server.OPERATION_BLOCKED, instance.actor.client_error(instance.op, "Too far away"))
