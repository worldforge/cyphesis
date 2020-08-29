# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).
import time

from mind.Goal import Goal


# goals for minds


def false(_): return False


def true(_): return True


class Delayed(Goal):
    """Will delay execution of sub goals until the specified time."""

    def __init__(self, time: float, sub_goals: list, desc="A delayed goal."):
        Goal.__init__(self, desc=desc, fulfilled=self.is_right_time, sub_goals=sub_goals)
        self.time = time

    def is_right_time(self, me):
        # Return "false" when the time is right
        is_right = time.time() < self.time
        return is_right


class OneShot(Goal):
    """Will remove itself after the first successful execution of its subgoals."""

    def __init__(self, sub_goals, desc="Executed once."):
        Goal.__init__(self, desc=desc, sub_goals=sub_goals)

    def check_goal_recursively(self, me, depth, debug_info):
        res, debug_info = super().check_goal_recursively(me, depth, debug_info)
        if res:
            self.irrelevant = True
        return res, debug_info


class DelayedOneShot(Goal):
    """Combines delayed execution with one shot. Useful when you want to perform one action once after a certain time."""

    def __init__(self, sub_goals, desc="Executed once after a delay"):
        Goal.__init__(self, desc=desc, sub_goals=[OneShot(sub_goals=[Delayed(time=time.time() + 1, sub_goals=sub_goals)])])


class Condition(Goal):
    """
    A conditional goal which first executes a function, and then sets the subgoals to one of two possibilities.
    If the condition function returns None then none of the subgoals will be executed.
    """

    def __init__(self, condition_fn, goals_true, goals_false, desc="condition"):
        Goal.__init__(self, desc=desc, fulfilled=self.assess_condition)
        self.condition_fn = condition_fn
        self.goals_true = goals_true
        self.goals_false = goals_false

    def assess_condition(self, me):
        result = self.condition_fn(me)
        if result is None:
            return True
        if result:
            self.sub_goals = self.goals_true
        else:
            self.sub_goals = self.goals_false
        return False


class Sequence(Goal):
    """A goal which will check on all subgoals in order."""

    def __init__(self, sub_goals, desc="Sequence of goals"):
        Goal.__init__(self, desc=desc, sub_goals=sub_goals)


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


def get_task(me, task_name):
    """Gets the task by the name from the 'tasks' property, if it exists."""
    tasks_prop = me.entity.get_prop_map('tasks')
    if tasks_prop and task_name in tasks_prop:
        return tasks_prop[task_name]
