#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from mind.goals.common.common import *
from mind.goals.common.move import move_me
from mind.goals.dynamic.DynamicGoal import DynamicGoal
from mind.goals.dynamic.add_unique_goal import add_unique_goal_by_perception

#class sell_things(add_unique_goal_by_perception):
    #def __init__(self, desc="add transaction goal"):
        #add_unique_goal_by_perception.__init__(self,
                                               #transaction,
                                               #trigger="sound_talk",
                                               #desc=desc)

class execute(DynamicGoal):
    def __init__(self, what, cost, desc="buy livestock by the kg"):
        DynamicGoal.__init__(self,
                             trigger="interlinguish_desire_verb3_vote_verb1",
                             desc=desc)
        self.cost=int(cost)
        self.what=what
    def event(self, me, op, say):
        print "Vote trigger called"
