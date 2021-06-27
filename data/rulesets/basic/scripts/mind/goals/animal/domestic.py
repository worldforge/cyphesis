# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2007 Al Riddoch (See the file COPYING for details).

import re

from atlas import Operation, Entity
from physics import Vector3D
from rules import Location

sowee_pattern = re.compile("[Ss]owee")

from mind.goals.dynamic.DynamicGoal import DynamicGoal


class Driven(DynamicGoal):
    """Move away from a herder when touched."""

    def __init__(self, desc="Move away when touched, as by a swineherd"):
        DynamicGoal.__init__(self, trigger="touch", desc=desc)

    def event(self, me, original_op, op):
        direction = me.steering.direction_to(me.map.get(op.from_).location)
        destination = Location()
        destination.velocity = -direction.unit_vector()
        return Operation("move", Entity(me.entity.id, location=destination))


class Summons(DynamicGoal):
    """Stop moving when the herder gives a cry."""

    def __init__(self, verb, desc="Come to a stop when commanded to"):
        DynamicGoal.__init__(self, trigger="sound_talk", desc=desc)

    def event(self, me, original_op, op):
        # FIXME Trigger this on interlinguish instead
        # FIXME Use the verb rather than the hardcoded RE
        talk_entity = op[0]
        if hasattr(talk_entity, "say"):
            say = talk_entity.say
            if sowee_pattern.match(say):
                destination = Location()
                destination.velocity = Vector3D(0, 0, 0)
                return Operation("move", Entity(me.entity.id, location=destination))
