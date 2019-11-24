# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

from physics import *

from mind.goals.common.misc_goal import *


class Survive(Goal):

    def __init__(self):
        Goal.__init__(self, desc="Survive by avoiding or fighting enemies",
                      subgoals=[self.move_to_loc])
