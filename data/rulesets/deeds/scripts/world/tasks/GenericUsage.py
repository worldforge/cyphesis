# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

import server
from atlas import Operation

from world.StoppableTask import StoppableTask


def use(instance):
    usage_name = instance.op.parent

    task = GenericUsage(instance, duration=5, tick_interval=1, name=usage_name.capitalize())

    instance.actor.send_world(Operation("sight", instance.op))

    return server.OPERATION_BLOCKED, instance.actor.start_task(usage_name, task)


class GenericUsage(StoppableTask):
    """ A generic usage class, for simple usages where we want to wait until sending the op to the target. """

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.start_action(self.name)

    def tick(self):
        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)

    def completed(self):
        # Send the op to the entity
        return Operation(self.op.parent, to=self.tool, id=self.actor.id)
