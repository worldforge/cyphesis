# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

from atlas import *
from world.utils import Usage

import server

def sow(instance):
    Usage.set_cooldown_on_attached(instance.tool, instance.actor)
    target_entity = instance.targets[0].entity

    if target_entity.is_reachable_for_other_entity(instance.actor, None, 0):
        task = Cultivate(instance)
        task.rate = 1 / 1.75
        task.tick_interval = 1.75
        task.name = "Cultivate"

        return (server.OPERATION_BLOCKED, instance.actor.start_task(task))
    else:
        print("Too far away.")
        return (server.OPERATION_BLOCKED)

class Cultivate(server.Task):
    """ A proof of concept task germinating seeds into plants."""
    def setup(self):
        """ Setup code, could do something """
        pass

    def tick(self):

        target = self.targets[0]
        entity = target.entity

        if not target:
            self.irrelevant()
            return
        if not entity.is_reachable_for_other_entity(self.actor, None, 0):
            self.irrelevant()
            return
        (valid, err_op) = self.usage.is_valid()
        if not valid:
            self.irrelevant()
            return err_op

        new_loc = entity.location.copy()
        #Create a small instance of the type this target germinates, and destroy the seed.
        create=Operation("create", Entity(parent = entity.props.germinates,
                                          scale = [0.1],
                                          location = new_loc,
                                          mode = "planted"), to = entity)
        set=Operation("set", Entity(entity.id, status=-1), to=entity)

        self.irrelevant()
        return (create, set)
