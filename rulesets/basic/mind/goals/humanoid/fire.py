#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from mind.goals.common.common import *
from mind.goals.common.move import move_me
from mind.goals.dynamic.add_unique_goal import add_unique_goal_by_perception

class extinguish_fire(Goal):
    def __init__(self, what):
        Goal.__init__(self,"extinguish fire",
                      self.fire_extinguished,
                      [move_me(what.location),self.extinguish])
        self.what=what
        self.vars=["what"]
    def fire_extinguished(self, me):
        fire=me.map.get(self.what.id)
        if not fire:
            self.irrelevant=1 #remove this goal
            return 1
        return 0
    def extinguish(self, me):
        return Operation("extinguish",Entity(self.what.id),to=self.what)

class add_extinguish_fire(add_unique_goal_by_perception):
    def __init__(self, desc="add extinguish fire goal"):
        add_unique_goal_by_perception.__init__(self,
                                               extinguish_fire,
                                               trigger="sight_burn",
                                               desc=desc)
