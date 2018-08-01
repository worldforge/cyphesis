# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2005 Al Riddoch (See the file COPYING for details).
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

from atlas import *
from world.utils import Usage

import server

def ignite(instance):
    Usage.set_cooldown_on_attached(instance.tool, instance.actor)
    target = instance.targets[0]
    if instance.actor.can_reach(target):
        return (server.OPERATION_BLOCKED, Operation("create", Entity(parent='fire', status=0.05, location=Location(target.entity, target.pos)), to=target.entity))
    else:
        return (server.OPERATION_BLOCKED, instance.actor.client_error(instance.op, "Too far away"))
