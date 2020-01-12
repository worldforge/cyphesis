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
            self.subgoals = self.goals_left
        else:
            self.subgoals = self.goals_right
        return False
