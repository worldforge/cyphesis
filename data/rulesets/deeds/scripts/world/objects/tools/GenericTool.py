# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

import server
from atlas import Operation, Entity

from world.StoppableTask import StoppableTask
from world.utils import Usage


def use(instance):
    usage_name = instance.op.parent

    task = Usage.delay_task_if_needed(Use(instance, duration=5, tick_interval=1, name=usage_name.capitalize()))
    Usage.set_cooldown_on_attached(instance.tool, instance.actor)

    instance.actor.send_world(Operation("sight", instance.op))

    return server.OPERATION_BLOCKED, instance.actor.start_task(usage_name, task)


class Use(StoppableTask):
    """ A generic tool usage class, for simple usages where we want to wait until sending the op to the target. """

    def setup(self, task_id):
        """ Setup code, could do something """
        self.start_action(self.name.lower())

    def tick(self):
        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)

    def completed(self):
        target = self.get_arg("targets", 0)
        entity = target.entity

        # Send the op to the entity
        return Operation(self.op.parent, Entity(pos=target.pos), to=entity, id=self.actor.id)
