# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

from .common import const
from physics import *
from rules import Location

from physics import Vector3D
from physics import Point3D

import entity_filter

from mind.Goal import Goal, goal_create
from mind.goals.common.common import *
from mind.goals.common.misc_goal import *
from random import *

import types


class Survive(Goal):

    def __init__(self):
        Goal.__init__(self, desc="Survive by avoiding or fighting enemies",
                      subgoals =[self.move_to_loc])
