#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

from world.objects.Character import Character
from misc import set_kw

class Settler(Character):
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
