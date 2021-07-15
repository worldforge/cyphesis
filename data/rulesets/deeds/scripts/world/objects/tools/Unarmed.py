# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

import server
from atlas import Operation, Entity

from world.StoppableTask import StoppableTask
from world.utils import Usage


def strike(instance):
    """Strike another entity with your body (fists, fangs, talons etc.)."""

    tick_interval = getattr(instance.tool.props, "cooldown_" + instance.op.parent)
    if tick_interval is not None:
        task = Usage.delay_task_if_needed(Fight(instance, tick_interval=tick_interval, name="Unarmed"))
    else:
        task = Usage.delay_task_if_needed(Fight(instance, name="Unarmed"))
    task_op = instance.actor.start_task('melee', task)

    return server.OPERATION_BLOCKED, task_op


class Fight(StoppableTask):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # Start in the cool down state; i.e. right after there has been a cooldown and there should either be a warmup or a direct strike.
        self.state = "cooldown"

    def do_strike(self):
        # Send sight even if we miss
        self.usage.actor.send_world(Operation("sight", self.usage.op))
        self.start_action("unarmed/strike")

        # If there's a cooldown we need to mark the actor
        cooldown = getattr(self.usage.tool.props, "cooldown_" + self.usage.op.parent)
        if cooldown and cooldown > 0.0:
            self.usage.tool.send_world(Operation('set',
                                                 Entity(self.usage.tool.id,
                                                        ready_at=server.world.get_time() + cooldown),
                                                 to=self.usage.tool.id))
            self.tick_interval = cooldown
            self.state = "cooldown"

        target = self.usage.get_arg("targets", 0)
        if target:
            if target.parent and target.parent.is_destroyed:
                self.irrelevant("Target is destroyed.")
                return server.OPERATION_BLOCKED

            # Take a swing/bite/render
            if self.usage.actor.can_reach(target):
                damage = self.usage.actor.get_prop_float("damage_" + self.usage.op.parent, 0.0)

                hit_op = Operation('hit', Entity(damage=damage, hit_type=self.usage.op.parent, id=self.usage.actor.id),
                                   to=target.entity)
                return server.OPERATION_BLOCKED, hit_op
            else:
                return server.OPERATION_BLOCKED, self.usage.actor.client_error(self.usage.op, "Target can not be reached.")
        else:
            print("No target")
        return server.OPERATION_BLOCKED

    def setup(self, task_id):
        return self.do_warmup_or_strike()

    def do_warmup(self):
        self.start_action("unarmed/warmup")

    def do_warmup_or_strike(self):
        if self.state == "cooldown":
            # Check if we should either wait a little for warmup phase, or if we should do the strike directly
            warmup = self.usage.actor.get_prop_float("warmup_" + self.usage.op.parent, 0.0)
            if warmup > 0:
                self.tick_interval = warmup
                self.state = "warmup"
                return self.do_warmup()
            else:
                return self.do_strike()
        else:
            # We have already done warmup; do the strike
            return self.do_strike()

    def tick(self):

        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)

        return self.do_warmup_or_strike()
