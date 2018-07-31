# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

from atlas import *
from world.utils import Usage

import server

def sow(tool, actor, op, targets, consumed):
    Usage.set_cooldown_on_attached(tool, actor)
    target_entity = targets[0].entity

    if target_entity.is_reachable_for_other_entity(actor, None, 0):
        oplist = Oplist()
        task = Cultivate(actor)
        task.rate = 1 / 1.75
        task.target_id = target_entity.id
        task.tick_interval = 1.75
        task.name = "Cultivate"
        actor.start_task(task, Operation('sow'), oplist)
        return (server.OPERATION_BLOCKED, oplist)
    else:
        print("Too far away.")
        return (server.OPERATION_BLOCKED)

class Cultivate(server.Task):
    """ A proof of concept task germinating seeds into plants."""
    def sow_operation(self, op):
        """ Op handler for sow op which activates this task """

        return (server.OPERATION_HANDLED)

    def tick_operation(self, op):

        target = server.world.get_object(self.target_id)

        if not target:
            self.irrelevant()
            return
        if not target.is_reachable_for_other_entity(self.character, None, 0):
            self.irrelevant()
            return

        new_loc = target.location.copy()
        #Create a small instance of the type this target germinates, and destroy the seed.
        create=Operation("create", Entity(parent = target.props.germinates,
                                          scale = [0.1],
                                          location = new_loc,
                                          mode = "planted"), to = target)
        set=Operation("set", Entity(target.id, status=-1), to=target)

        self.irrelevant()
        return (server.OPERATION_BLOCKED, create, set)
