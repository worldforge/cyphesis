# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2000 Aloril (See the file COPYING for details).

from mind.Goal import Goal


class DynamicGoal(Goal):
    """Base class for dynamic or trigger goals.

    A dynamic goal is one which is activated by an event or trigger,
    usually the sight of an event, or a trigger language construct
    in a talk operation."""

    def __init__(self, desc="some dynamic goal", trigger=None, **kw):
        kw['desc'] = desc
        Goal.__init__(*(self,), **kw)
        self.trigger_data = trigger
        self.vars.append("trigger_data")

    def trigger(self):
        return self.trigger_data

    def triggering_goals(self):
        return [self]

    def event(self, me, original_op, op):
        """this is called when trigger even is received"""
        # redefine this
        pass
