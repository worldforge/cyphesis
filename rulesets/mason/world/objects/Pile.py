#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005 Al Riddoch (See the file COPYING for details).

from atlas import *

import server

class Pile(server.Thing):
    """Using a pile to extinguish the fire"""
    def disperse_operation(self, op):
        to_ = op[0].id
        # print "someoperation"
        return Operation("extinguish",op[0],to=to_)
