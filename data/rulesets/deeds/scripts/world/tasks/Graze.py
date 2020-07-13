# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

import server
from atlas import Operation, Entity

from world.StoppableTask import StoppableTask


def graze(instance):
    usage_name = instance.op.parent

    task = Graze(instance, duration=2, tick_interval=1, name=usage_name.capitalize())

    instance.actor.send_world(Operation("sight", instance.op))

    return server.OPERATION_BLOCKED, instance.actor.start_task(usage_name, task)


class Graze(StoppableTask):
    """ Eat from the ground. """

    def tick(self):
        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)

    def completed(self):
        target = self.usage.get_arg("targets", 0)
        if self.usage.actor.can_reach(target):
            mass = self.usage.actor.props.mass * 0.01
            return Operation("consume", Entity(consume_type='graze', pos=target.pos, mass=mass), to=target.parent.id, from_=self.actor.id)
