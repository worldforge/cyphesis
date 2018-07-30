# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

from atlas import *
from world.utils import Usage

import server

#TODO: make it into a task which takes some time to complete

def sow(tool, actor, op, targets, consumed):
    Usage.set_cooldown_on_attached(tool, actor)
    target_entity = targets[0].entity
    if target_entity.is_reachable_for_other_entity(actor, None, 0):
        new_loc = target_entity.location.copy()
        create=Operation("create", Entity(parent = target_entity.props.germinates,
                                          scale = [0.1],
                                          location = new_loc,
                                          mode = "planted"), to = target_entity)
        set=Operation("set", Entity(target_entity.id, status=-1), to=target_entity)
        return (server.OPERATION_BLOCKED, create, set)
    else:
        print("Too far away.")
        return (server.OPERATION_BLOCKED)
