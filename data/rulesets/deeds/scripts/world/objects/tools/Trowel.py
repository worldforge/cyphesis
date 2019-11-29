# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

import server
from atlas import Operation, Entity

from world.utils import Usage


def sow(instance):
    Usage.set_cooldown_on_attached(instance.tool, instance.actor)

    task = Cultivate(instance, duration=11, tick_interval=1, name="Cultivate")

    instance.actor.send_world(Operation("sight", instance.op))

    return server.OPERATION_BLOCKED, instance.actor.start_task('cultivate', task)


class Cultivate(server.Task):
    """ A proof of concept task germinating seeds into plants."""

    def setup(self):
        """ Setup code, could do something """
        pass

    def tick(self):
        target = self.get_arg("targets", 0)

        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)

    def completed(self):
        target = self.get_arg("targets", 0)
        entity = target.entity

        new_loc = entity.location.copy()
        # Create a small instance of the type this target germinates, and destroy the seed.
        create_op = Operation("create", Entity(parent=entity.props.germinates,
                                               scale=[0.1],
                                               location=new_loc,
                                               mode="planted"), to=entity)
        set_op = Operation("set", Entity(entity.id, status=-1), to=entity)

        return create_op, set_op
