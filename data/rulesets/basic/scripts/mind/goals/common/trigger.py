# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2007 Al Riddoch (See the file COPYING for details).

from random import uniform

from atlas import Operation, Entity
from rules import Location

from mind.goals.dynamic.DynamicGoal import DynamicGoal


class Chase(DynamicGoal):
    """Chase after something that has moved."""

    def __init__(self, kinds, desc="Chase after things."):
        DynamicGoal.__init__(self, trigger="sight_move", desc=desc)
        self.kinds = kinds

    def event(self, me, original_op, op):
        # FIXME Now that this code is trigger goal, has this update been done?
        other = me.map.update(op[0], op.get_seconds())
        if other.id == me.entity.id:
            return
        # target=op[0].location.copy()
        if other.parent.id != me.entity.parent.id:
            return
        if hasattr(other, "type") and other.type[0] not in self.kinds:
            return
        destination = other.location.pos
        distance = destination.distance(me.entity.location.pos)
        if distance < 1:
            return
        # CHeat, random chance that it ignores movement
        if uniform(0, 30 / distance) < 1:
            return
        target = Location(me.entity.parent)
        velocity = destination - me.entity.location.pos
        if velocity.mag() == 0:
            return
        target.velocity = velocity.unit_vector()
        target.pos = destination
        return Operation("move", Entity(me.entity.id, location=target))
