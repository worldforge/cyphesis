# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).
from mind.Goal import Goal


# goals for minds


def false(_): return False


def true(_): return True


class Condition(Goal):
    """A conditional goal which first executes a method, and then sets the subgoals to one of two possibilities."""

    def __init__(self, condition_fn, goals_left, goals_right, desc="condition"):
        Goal.__init__(self, desc=desc, fulfilled=self.assess_condition)
        self.condition_fn = condition_fn
        self.goals_left = goals_left
        self.goals_right = goals_right

    def assess_condition(self, me):
        result = self.condition_fn(me)
        if result is None:
            return True
        if result:
            self.sub_goals = self.goals_left
        else:
            self.sub_goals = self.goals_right
        return False


def get_reach(me):
    reach = 0
    own_reach = me.entity.get_prop_float('reach')
    if own_reach:
        reach += own_reach

    attached_current = me.get_attached_entity("hand_primary")
    if attached_current:
        attached_reach = attached_current.get_prop_float('reach')
        if attached_reach:
            reach += attached_reach

    return reach


def get_focused_location(me, what):
    thing = get_focused_thing(me, what)
    if thing:
        return thing.location
    return None


def get_focused_thing(me, what):
    focus_id = me.get_knowledge('focus', what)
    if focus_id is None:
        return None
    thing = me.map.get(focus_id)
    if thing is None:
        me.remove_knowledge('focus', what)
        return None
    return thing
