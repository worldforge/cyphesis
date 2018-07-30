# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

from atlas import *

import server
from world.utils import Usage

def strike(tool, actor, op, targets, consumed):
    # Check that we can reach the target with our weapon
    extraReach = 0.0
    if tool.props.reach:
        extraReach = tool.props.reach

    # If there's a cooldown we need to mark the actor
    Usage.set_cooldown_on_attached(tool, actor)

    # Sword only handles one target
    target = targets[0]
    # Ignore pos
    if target.entity.is_reachable_for_other_entity(actor, None, extraReach):
        print("Strike!")
        damage = 0
        if tool.props.damage:
            damage = tool.props.damage
        hitOp = Operation('hit', Entity(damage=damage), to=target.entity)
        return (server.OPERATION_BLOCKED, hitOp, Operation('sight', hitOp))
    else:
        print("Too far away.")
        return (server.OPERATION_BLOCKED)
