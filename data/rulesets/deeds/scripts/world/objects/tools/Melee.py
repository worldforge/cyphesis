# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

import server
from atlas import Operation, Entity

from world.StoppableTask import StoppableTask
from world.utils import Usage


def strike(instance):
    """Strike another entity with a weapon."""

    # Melee weapons only handles one target
    tick_interval = instance.tool.get_prop_float("cooldown")
    if tick_interval is not None:
        task = Usage.delay_task_if_needed(Melee(instance, tick_interval=tick_interval, name="Melee"))
    else:
        task = Usage.delay_task_if_needed(Melee(instance, name="Melee"))
    task_op = instance.actor.start_task('melee', task)

    return server.OPERATION_BLOCKED, task_op


class Melee(StoppableTask):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # Start in the cool down state; i.e. right after there has been a cooldown and there should either be a warmup or a direct strike.
        self.state = "cooldown"

    def do_strike(self):
        # Send sight even if we miss
        self.usage.actor.send_world(Operation("sight", self.usage.op))
        self.start_action("melee/strike")

        # If there's a cooldown we need to mark the actor
        cooldown = Usage.set_cooldown_on_attached(self.usage.tool, self.usage.actor)
        if cooldown and cooldown > 0.0:
            # Schedule next tick to cooldown time, and set in state "cooldown".
            self.tick_interval = cooldown
            self.state = "cooldown"

        target = self.usage.get_arg("targets", 0)
        if target:
            if target.parent and target.parent.is_destroyed:
                self.irrelevant("Target is destroyed.")
                return server.OPERATION_BLOCKED

            # Take a swing
            # Check that we can reach the target with our weapon
            extra_reach = 0.0
            if self.usage.tool.props.reach:
                extra_reach = self.usage.tool.props.reach

            if self.usage.actor.can_reach(target, extra_reach):
                damage = 0
                if self.usage.tool.props.damage:
                    damage = self.usage.tool.props.damage
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

    def do_warmup_or_strike(self):
        if self.state == "cooldown":
            # Check if we should either wait a little for warmup phase, or if we should do the strike directly
            warmup = self.usage.tool.get_prop_float("warmup", 0.0)
            if warmup > 0:
                self.tick_interval = warmup
                self.state = "warmup"
                return self.do_warmup()
            else:
                return self.do_strike()
        else:
            # We have already done warmup; do the strike
            return self.do_strike()

    def do_warmup(self):
        self.start_action("melee/warmup")

    def tick(self):

        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)

        return self.do_warmup_or_strike()
