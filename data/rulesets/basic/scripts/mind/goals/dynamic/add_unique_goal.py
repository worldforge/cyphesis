# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).

from mind.goals.dynamic.DynamicGoal import DynamicGoal


############################ ADD UNIQUE ####################################


class AddUniqueGoal(DynamicGoal):
    """Base class for dynamic goals which add a regular goal."""

    def __init__(self, goal_to_add, desc="add_unique_goal", **kw):
        kw['desc'] = desc
        DynamicGoal.__init__(*(self,), **kw)
        self.goal_class = goal_to_add
        self.vars.append("goal_class")

    def event(self, me, original_op, op):
        """adds goal using given operation and execute that goal:
           first let see if we already have added this goal
           then we generate and add goal if it doesn't exist
           and finally we execute goal"""
        if not self.check_event(me, original_op, op):
            return
        if len(self.sub_goals) == 0:
            goal = self.make_goal_instance(me, self.goal_class, original_op, op)
            if goal:
                self.sub_goals.append(goal)
        if len(self.sub_goals) != 0:
            return self.sub_goals[0].check_goal(me)

    def make_goal_instance(self, me, goal_class, original_op, op):
        # override this!
        return goal_class(me, original_op, op)

    def check_event(self, me, original_op, op):
        """override this to check to see whether we really want to add a goal"""
        return 1


class AddUniqueGoalByPerception(AddUniqueGoal):
    """Base class for dynamic goals which add a regular goal base on
       a perception trigger."""

    def make_goal_instance(self, me, goal_class, original_op, op):
        object = me.map.get(op[0].id)
        if object:
            return goal_class(object)
