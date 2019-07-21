# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

from atlas import Operation, Entity
from world.utils import Usage

import server


def dig(instance):
    print('dig')
    Usage.set_cooldown_on_attached(instance.tool, instance.actor)

    task = Dig(instance, duration=5, tick_interval=1, name="Dig")

    instance.actor.send_world(Operation("sight", instance.op))

    return server.OPERATION_BLOCKED, instance.actor.start_task('dig', task)


class Dig(server.Task):
    """ A proof of concept task germinating seeds into plants."""

    def setup(self):
        """ Setup code, could do something """
        pass

    def tick(self):

        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)

    def completed(self):
        target = self.get_arg("targets", 0)
        entity = target.entity

        new_loc = entity.location.copy()
        # Send a dig op to the entity
        dig_op = Operation("dig", Entity(pos=target.pos), to=entity)

        return dig_op
