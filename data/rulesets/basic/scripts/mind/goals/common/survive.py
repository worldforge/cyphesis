# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

from mind.goals.common.misc_goal import *
from physics import *


class Survive(Goal):

    def __init__(self):
        Goal.__init__(self, desc="Survive by avoiding or fighting enemies",
                      sub_goals=[self.move_to_loc])
