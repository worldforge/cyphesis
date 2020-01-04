# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import Operation, Entity

from mind.Goal import Goal
from mind.goals.common.move import MoveMe
from mind.goals.dynamic.add_unique_goal import AddUniqueGoalByPerception


class ExtinguishFire(Goal):
    """Put out a specified fire."""

    def __init__(self, what):
        Goal.__init__(self, "extinguish fire",
                      self.fire_extinguished,
                      [MoveMe(what.location), self.extinguish])
        self.what = what
        self.vars = ["what"]

    def fire_extinguished(self, me):
        fire = me.map.get(self.what.id)
        if not fire:
            self.irrelevant = 1  # remove this goal
            return 1
        return 0

    def extinguish(self, me):
        return Operation("extinguish", Entity(self.what.id), to=self.what)


class AddExtinguishFire(AddUniqueGoalByPerception):
    """Respond to spotting a fire by adding a goal to extinguish it."""

    def __init__(self, desc="add extinguish fire goal"):
        AddUniqueGoalByPerception.__init__(self,
                                           ExtinguishFire,
                                           trigger="sight_burn",
                                           desc=desc)
