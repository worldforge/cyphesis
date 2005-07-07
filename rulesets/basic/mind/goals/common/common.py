#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

#if const.server_python:
#from world.physics.Vector3D import Vector3D
#else:
from Vector3D import Vector3D

#from common import const
from mind.Goal import Goal
from mind.NPCMind import *

#goals for minds

def false(me): return 0
def true(me): return 1

