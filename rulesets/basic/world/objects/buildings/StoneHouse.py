#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Kosh (See the file COPYING for details).

from atlas import *

from world.objects.Thing import Thing
from common import const
from world import probability
from world.objects.buildings.House import House

class Stonehouse(House):
    """This class is a stone house it does not burn"""
