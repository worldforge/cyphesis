#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from mind.Goal import Goal
from mind.goals.dynamic.DynamicGoal import DynamicGoal

############################ ADD UNIQUE ####################################


class add_unique_goal(DynamicGoal):
    def __init__(self, goal_to_add, desc="add_unique_goal", **kw):
        kw['desc'] = desc
        apply(DynamicGoal.__init__,(self,),kw)
        self.goal_class = goal_to_add
        self.vars.append("goal_class")
    def event(self, me, original_op, op):
        """adds goal using given operation and execute that goal:
           first let see if we already have added this goal
           then we generate and add goal if it doesn't exist
           and finally we execute goal"""
        if not self.check_event(me, original_op, op):
            return
        goal = self.find_goal(me, self.goal_class)
        if not goal:
            goal = self.make_goal_instance(me, self.goal_class, original_op, op)
            if goal:
                self.add_goal(me, goal)
        if goal:
            return goal.check_goal(me, me.time)
    def find_goal(self, me, goal_class):
        for goal in me.goals:
            if goal.__class__==goal_class:
                return goal
    def add_goal(self, me, goal):
        """add goal into mind"""
        me.goals.insert(0,goal)
    def make_goal_instance(self, me, goal_class, original_op, op):
        #override this!
        return goal_class(me, original_op, op)
    def check_event(self, me, original_op, op):
        """override this to check to see whether we really want to add a goal"""
        return true

class add_unique_goal_by_perception(add_unique_goal):
    def make_goal_instance(self, me, goal_class, original_op, op):
        object=me.map.get(op[0].id)
        if object:
            return goal_class(object)
