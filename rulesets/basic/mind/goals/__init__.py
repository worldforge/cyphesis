#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
import sys

#All types of minds uses these goals
from mind.goals.common.move import *
from mind.goals.common.misc_goal import *

# Only humanoids use these goals
from mind.goals.humanoid.fire import *
from mind.goals.humanoid.build_home import *
from mind.goals.humanoid.transaction import *
from mind.goals.humanoid.mason import *
from mind.goals.humanoid.werewolf import *

# Only animals use these goals
from mind.goals.animal.herd import *
