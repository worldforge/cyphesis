#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
try:
  from random import *
except ImportError:
  from whrandom import *

import rulesets

class Statistics(rulesets.Statistics):
    """A very simple Statistics example."""
    def __init__(self, entity):
        rulesets.Statistics.__init__(self, entity)
    def attribute(self, name):
        if name == "attack":
            return 1
        if name == "defence":
            return 1
        if name == "strength":
            if hasattr(self.character, 'mass'):
                return self.character.mass
            else:
                return 0
