# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2005 Al Riddoch (See the file COPYING for details).
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

import server
from atlas import Operation, Entity
from rules import Location

from world.utils import Usage


def ignite(instance):
    Usage.set_cooldown_on_attached(instance.tool, instance.actor)
    target = instance.get_arg("targets", 0)
    if instance.actor.can_reach(target):
        return server.OPERATION_BLOCKED, \
               Operation("create",
                         Entity(parent='fire', status=0.05, location=Location(target.entity), pos=[0, 0, 0]),
                         to=target.entity)
    else:
        return server.OPERATION_BLOCKED, instance.actor.client_error(instance.op, "Too far away")
