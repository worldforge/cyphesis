# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

import server
from atlas import Operation, Entity

from world.StoppableTask import StoppableTask


def strike(instance):
    """Strike another entity with your fists."""

    # If there's a cooldown we need to mark the actor
    cooldown = getattr(instance.tool.props, "cooldown_" + instance.op.parent)
    if cooldown and cooldown > 0.0:
        instance.tool.send_world(Operation('set',
                                           Entity(instance.tool.id,
                                                  ready_at=server.world.get_time() + cooldown),
                                           to=instance.tool.id))

    # Send sight even if we miss
    instance.actor.send_world(Operation("sight", instance.op))

    # Unarmed strike only handles one target
    target = instance.get_arg("targets", 0)
    # Ignore pos
    if target:
        if target.parent and target.parent.is_destroyed:
            return server.OPERATION_BLOCKED
        if cooldown:
            task = Fight(instance, tick_interval=cooldown, name="Fight")
            task_op = instance.actor.start_task('melee', task)
        else:
            task_op = None

        if instance.actor.can_reach(target):
            damage = 0
            damage_attr = getattr(instance.actor.props, "damage_" + instance.op.parent)
            if damage_attr:
                damage = damage_attr
            hit_op = Operation('hit', Entity(damage=damage, hit_type=instance.op.parent, id=instance.actor.id),
                               to=target.entity)
            return server.OPERATION_BLOCKED, hit_op, Operation('sight', hit_op), task_op
        else:
            return server.OPERATION_BLOCKED, instance.actor.client_error(instance.op, "Too far away"), task_op
    else:
        return server.OPERATION_BLOCKED


class Fight(StoppableTask):

    def tick(self):

        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)

        self.usage.actor.send_world(Operation("sight", self.usage.op))

        target = self.usage.get_arg("targets", 0)
        instance = self.usage

        # Ignore pos
        if target:
            if target.parent and target.parent.is_destroyed:
                self.irrelevant("Target is destroyed.")
                return server.OPERATION_BLOCKED
            if instance.actor.can_reach(target):
                damage = 0
                damage_attr = getattr(instance.actor.props, "damage_" + instance.op.parent)
                if damage_attr:
                    damage = damage_attr
                hit_op = Operation('hit', Entity(damage=damage, hit_type=instance.op.parent, id=instance.actor.id),
                                   to=target.entity)
                return server.OPERATION_BLOCKED, hit_op, Operation('sight', hit_op)
            else:
                return server.OPERATION_BLOCKED, instance.actor.client_error(instance.op, "Target can not be reached.")
        else:
            return server.OPERATION_BLOCKED
