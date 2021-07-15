# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

import entity_filter
import server
from atlas import Operation, Entity, Oplist
from physics import Vector3D, Quaternion

from world.StoppableTask import StoppableTask
from world.utils import Usage

arrow_filter = entity_filter.Filter("entity instance_of types.arrow")


def shoot_in_direction(direction, instance, res):
    Usage.set_cooldown_on_attached(instance.tool, instance.actor)

    arrows = instance.actor.find_in_contains(arrow_filter)
    if len(arrows):
        direction.normalize()

        # Adjust the start position of the arrow, so it's outside of the actor, at mid height
        start_adjust = Vector3D(direction)
        start_adjust.y = 0
        start_adjust.normalize()
        start_adjust.y = instance.actor.location.bbox.high_corner.y * 0.8

        new_loc = instance.actor.location.copy()
        new_loc.pos += start_adjust

        new_loc.orientation = Quaternion(Vector3D(0, 0, 1), direction, Vector3D(1, 0, 0))

        mode_data = {"mode": "projectile", "$eid": instance.actor.id, "extra": {"damage": 20}}

        # TODO: match with animation in client
        res.append(instance.actor.start_action("bow/releasing", 1))
        res.append(Operation("move", Entity(arrows[0].id,
                                            location=new_loc,
                                            velocity=direction * 60,
                                            mode="projectile",
                                            mode_data=mode_data),
                             to=arrows[0].id))


def shoot(instance):
    res = Oplist()
    direction = instance.get_arg("direction", 0)

    shoot_in_direction(direction, instance, res)

    return server.OPERATION_BLOCKED, res


def draw(instance):
    usage_name = instance.op.parent

    warmup = instance.tool.get_prop_float("warmup", 1.0)
    task = Usage.delay_task_if_needed(DrawBow(instance, tick_interval=warmup, name="Draw"))

    instance.actor.send_world(Operation("sight", instance.op))

    return server.OPERATION_BLOCKED, instance.actor.start_task(usage_name, task)


class DrawBow(StoppableTask):
    """
    Starts drawing the bow. Once it's drawn an additional "release" usage is added to the task.
    """

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.is_ready = False

    def setup(self, task_id):
        # TODO: match with animation in client
        self.start_action("bow/drawing")

    def tick(self):
        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)

        if not self.is_ready:
            # TODO When updating usages, or progress, the enveloping system should take care of checking that the Task (and thus the TaskProperty) has changed. No need for "actor.update_task()"
            self.usages = [{"name": "release", "params": {"direction": {"type": "direction"}}}] + self.usages
            self.is_ready = True
            return self.actor.update_task()

    def release_usage(self, args):
        res = Oplist()

        self.irrelevant()
        if self.is_ready:
            direction = args["direction"][0]
            if direction is not None:
                shoot_in_direction(direction, self.usage, res)
        return server.OPERATION_BLOCKED, res
