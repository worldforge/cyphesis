# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

from atlas import Operation, Entity, Oplist
from physics import square_distance
from rules import Location
from mind.Goal import Goal
from mind.goals.common.move import avoid
from mind.goals.common.misc_goal import spot_something, move_me_to_focus
import entity_filter

import time
import types


class Fight(Goal):
    """Fight enemies in range"""

    # TODO: Make entity first select weapon, and then adjust strategy depending on weapon.
    # TODO: I.e. when using a ranged weapon the entity should keep range.

    def __init__(self, what="", range=30):
        Goal.__init__(self, "fight something",
                      self.none_in_range,
                      [spot_something(what=what, range=range),
                       move_me_to_focus(what=what, radius=2),
                       #                       hunt_for(what=what, range=range, proximity=3),
                       self.fight])
        self.what = what
        self.filter = entity_filter.Filter(what)
        self.range = range
        self.square_range = range * range
        self.vars = ["what", "range"]

    def none_in_range(self, me):
        thing_all = me.map.find_by_filter(self.filter)
        for thing in thing_all:
            if square_distance(me.entity.location, thing.location) < self.square_range:
                return 0
        return 1

    def fight(self, me):
        id = me.get_knowledge('focus', self.what)
        if id is None:
            print("No focus target")
            return
        enemy = me.map.get(id)
        if enemy is None:
            print("No target")
            me.remove_knowledge('focus', self.what)
            return
        print("Punching")
        return Operation("use", Operation("punch", Entity(me.entity.id, targets=[Entity(enemy.id)])))


class FightOrFlight(Goal):
    """Fight or flee from enemies in range"""

    def __init__(self, range=30):
        Goal.__init__(self, desc="fight or flee from enemies",
                      subgoals=[Fight(what="memory.disposition <= -1 && memory.threat < 0", range=range),
                                avoid(what="memory.disposition <= -1 && memory.threat > 0", range=range)])
