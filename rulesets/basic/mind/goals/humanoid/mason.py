#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from mind.goals.common.common import *
from mind.goals.common.misc_goal import keep
from mind.goals.dynamic.DynamicGoal import DynamicGoal
from mind.goals.dynamic.add_unique_goal import add_unique_goal

class keep_livestock(keep):
    def __init__(self, what, where, call):
        keep.__init__(self, what, where)
        self.call=call
    def keep_it(self,me):
        thing_all=me.find_thing(self.what)
        for thing in thing_all:
            if thing.location.velocity.is_valid() and thing.location.velocity.square_mag() > 0.1:
                return Operation("talk", Entity(say=self.call), to=me)
        return keep.keep_it(self,me)

class welcome(DynamicGoal):
    def __init__(self, message, player, desc="welcome new players"):
        DynamicGoal.__init__(self,
                             trigger="sight_create",
                             desc=desc)
        self.player=player
        self.message=message
    def event(self, me, original_op, op):
        obj = me.map.update(op[0], op.getSeconds())
        if original_op.from_==self:
            self.add_thing(obj)
        if obj.type[0]==self.player:
            return Operation("talk", Entity(say=self.message), to=me)

class help(Goal):
    def __init__(self, messages):
        Goal.__init__(self,
                      "help",
                      self.message_complete,
                      [self.give_help])
        self.iter=0
        self.count=len(messages)
        self.messages=messages
        self.vars=["iter", "count", "messages"]
    def message_complete(self, me):
        if self.iter>=self.count:
            self.irrelevant=1
            return 1
        return 0
    def give_help(self, me):
        message = self.messages[self.iter]
        self.iter+=1
        return Operation("talk", Entity(say=message))

class add_help(add_unique_goal):
    def __init__(self, messages, desc="help people out"):
        add_unique_goal.__init__(self,
                                 help,
                                 trigger="touch",
                                 desc=desc)
        self.messages=messages
    def make_goal_instance(self, me, goal_class, original_op, op):
        return goal_class(self.messages)
