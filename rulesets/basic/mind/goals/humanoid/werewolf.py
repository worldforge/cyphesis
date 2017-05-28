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

# This goal collects votes
class elect(DynamicGoal):
    def __init__(self, desc="count election votes"):
        DynamicGoal.__init__(self,
                             trigger="interlinguish_desire_verb3_vote_verb1",
                             desc=desc)
    def event(self, me, op, say):
        print "Vote trigger called"

class count_players(DynamicGoal):
    def __init__(self, desc="count players"):
        print "INIT count goal"
        DynamicGoal.__init__(self, trigger="sight_create", desc=desc)
        self.players=[]
    def event(self, me, original_op, op):
        print "count trigger called"
        ent=op[0]
        if ent.type != "villager":
            return
        entid=ent.id
        self.players.append(entid)
        me.player_list = self.players

class decount_players(DynamicGoal):
    def __init__(self, desc="count players"):
        print "INIT decount goal"
        DynamicGoal.__init__(self, trigger="sight_delete", desc=desc)
    def event(self, me, original_op, op):
        print "decount trigger called"
        entid=op[0].id
        me.player_list.remove(entid)
