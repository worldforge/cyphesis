# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).
# Copyright (C) 2001 Al Riddoch (See the file COPYING for details).
import string

import ai
import entity_filter
from atlas import Operation, Entity

from mind.Goal import Goal
from mind.goals.common.misc_goal import Keep, DelayedOneShot
from mind.goals.dynamic.add_unique_goal import AddUniqueGoal


class KeepLivestock(Keep):
    """Keep livestock that we own in a given location, calling them if required."""

    def __init__(self, what, where, call):
        Keep.__init__(self, what, where)
        self.call = call

    def keep_it(self, me):
        thing_all = me.find_thing(self.what)
        for thing in thing_all:
            if thing.location.velocity.is_valid() and thing.location.velocity.sqr_mag() > 0.1:
                return Operation("talk", Entity(say=self.call))
        return Keep.keep_it(self, me)


class Welcome(Goal):
    """Welcome entities of a given type that are created nearby."""

    def __init__(self, message: string, what: string, distance=10):
        Goal.__init__(self, desc="welcome new players")
        self.what = what
        self.message = message
        self.filter = entity_filter.Filter(self.what)
        self.distance = distance

    def entity_appears(self, me, entity):
        # We need to check that the entity appeared close to us.
        # That means it was spawned. An existing entity which approached us would send the appearance
        # when it was much farther away, and can thus be ignored.
        if me.match_entity(self.filter, entity):
            distance_to_thing = me.steering.distance_to(entity, ai.CENTER, ai.CENTER)
            if distance_to_thing is None:
                print("Could not get distance to newly created entity which matches the filter for Welcome.")
            else:
                if distance_to_thing < self.distance:
                    welcome_goal = lambda me_: self.send_welcome(me_, entity)
                    self.sub_goals.append(DelayedOneShot(sub_goals=[welcome_goal]))
                    return me.face(entity)

    def send_welcome(self, me, entity):
        return Operation("talk", Entity(say=self.message)) + me.face(entity)


class Help(Goal):
    """Provide a sequence of help messages to a target."""

    def __init__(self, messages, responses, target):
        Goal.__init__(self,
                      "help",
                      self.message_complete,
                      [self.give_help])
        self.iter = 0
        self.count = len(messages)
        self.messages = messages
        self.responses = responses
        self.target = target
        self.vars = ["iter", "count", "messages", "responses", "target"]

    def message_complete(self, me):
        if self.iter >= self.count:
            self.irrelevant = 1
            return 1
        return 0

    def give_help(self, me):
        # Check that the target hasn't disappeared
        target_entity = me.map.get(self.target)
        if target_entity is None:
            self.irrelevant = 1
            return

        message = self.messages[self.iter]
        self.iter += 1
        if self.iter == self.count - 1 and len(self.responses) != 0:
            ent = Entity(say=message, responses=self.responses)
        else:
            ent = Entity(say=message)
        return Operation("talk", ent) + me.face(target_entity)


class AddHelp(AddUniqueGoal):
    """Set off a help goal if we get a touch operation."""

    def __init__(self, messages=None, responses=None):
        AddUniqueGoal.__init__(self,
                               Help,
                               trigger="touch",
                               desc="help people out")
        self.messages = messages
        self.responses = responses

    def make_goal_instance(self, me, goal_class, original_op, op):
        return goal_class(self.messages, self.responses, original_op.from_)
