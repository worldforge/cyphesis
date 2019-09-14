# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

from atlas import Operation, Entity

import server
from world.utils import Usage
from world.StoppableTask import StoppableTask


def strike(instance):
    """Strike another entity with a weapon."""

    # Check that we can reach the target with our weapon
    extra_reach = 0.0
    if instance.tool.props.reach:
        extra_reach = instance.tool.props.reach

    # If there's a cooldown we need to mark the actor
    Usage.set_cooldown_on_attached(instance.tool, instance.actor)

    # Send sight even if we miss
    instance.actor.send_world(Operation("sight", instance.op))

    # Melee weapons only handles one target
    target = instance.get_arg("targets", 0)
    if target:
        cooldown_prop = instance.tool.props.cooldown
        if cooldown_prop:
            task = Melee(instance, tick_interval=cooldown_prop, name="Melee")
            task_op = instance.actor.start_task('melee', task)
        else:
            task_op = None

        # Ignore pos
        if instance.actor.can_reach(target, extra_reach):
            damage = 0
            if instance.tool.props.damage:
                damage = instance.tool.props.damage
            hit_op = Operation('hit', Entity(damage=damage, hit_type=instance.op.parent, id=instance.actor.id), to=target.entity)
            return server.OPERATION_BLOCKED, hit_op, task_op
        else:
            return server.OPERATION_BLOCKED, instance.actor.client_error(instance.op, "Too far away"), task_op
    else:
        return server.OPERATION_BLOCKED


class Melee(StoppableTask):

    def tick(self):

        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)

        self.usage.actor.send_world(Operation("sight", self.usage.op))

        target = self.usage.get_arg("targets", 0)
        if target:
            # Take a swing
            extra_reach = 0.0
            if self.usage.tool.props.reach:
                extra_reach = self.usage.tool.props.reach

            if self.usage.actor.can_reach(target, extra_reach):
                damage = 0
                if self.usage.tool.props.damage:
                    damage = self.usage.tool.props.damage
                hit_op = Operation('hit', Entity(damage=damage, hit_type=self.usage.op.parent, id=self.usage.actor.id), to=target.entity)
                return server.OPERATION_BLOCKED, hit_op, Operation('sight', hit_op)
            else:
                return server.OPERATION_BLOCKED, self.usage.actor.client_error(self.usage.op, "Too far away")
        return server.OPERATION_BLOCKED
