#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
try:
  from random import *
except ImportError:
  from whrandom import *

import rules

class Statistics(rules.Statistics):
    """A very simple Statistics example."""
