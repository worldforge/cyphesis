# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).
import random
from random import randint

import entity_filter
import server
from atlas import Operation, Entity, Oplist

from world.StoppableTask import StoppableTask
from world.utils import Usage


def fish(instance):
    usage_name = instance.op.parent

    task = Usage.delay_task_if_needed(Fishing(instance, tick_interval=2, name="Fishing"))

    instance.actor.send_world(Operation("sight", instance.op))

    return server.OPERATION_BLOCKED, instance.actor.start_task(usage_name, task)


class Fishing(StoppableTask):
    """A fishing task. The player needs to have a fishing rod attached, and have a worm ('annelid') in it's inventory.
    At each tick there's a chance of a fish biting into the worm. This will be sent to the client as an imaginary op.
    If the user stops fishing when this happens a fish is caught, and the worm is consumed.
    """

    fishes = ['bass', 'tuna', 'bluegill']

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.fish_on_hook = False

    def setup(self, task_id):
        self.start_action("fishing")

    def tick(self):

        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)

        if random.uniform(0, 100) < 10:
            self.fish_on_hook = True
            return server.OPERATION_HANDLED, Operation("imaginary", Entity(description="You feel something nibbling."),
                                                       to=self.usage.actor.id, from_=self.usage.actor.id)
        else:
            self.fish_on_hook = False
        return server.OPERATION_HANDLED

    def stop_usage(self, args):
        # Check that the usage still is valid
        (valid, _) = self.usage.is_valid()
        self.irrelevant()
        res = Oplist()
        if valid and self.fish_on_hook:
            worms = self.usage.actor.find_in_contains(entity_filter.Filter("entity instance_of types.annelid"))
            if len(worms):
                fish_type = self.fishes[randint(0, len(self.fishes) - 1)]
                res.append(Operation("create", Entity(parent=fish_type, loc=self.usage.actor.id, mind=None),
                                     to=self.usage.tool))
                # Delete the worm
                res.append(Operation("delete", Entity(id=worms[0].id), to=worms[0].id))
                res.append(Operation("imaginary", Entity(description="You caught a {}.".format(fish_type)),
                                     to=self.usage.actor.id, from_=self.usage.actor.id))
        return server.OPERATION_HANDLED, res
