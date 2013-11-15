#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005 Al Riddoch (See the file COPYING for details).

from atlas import *

import server

class Trowel(server.Thing):
    """This is tool for planting seeds"""
    def sow_operation(self, op):
        to_ = op[0].id
        if not to_:
            return self.error(op,"To is undefined object")
        return Oplist(Operation("germinate",op[0],Entity(op.from_),to=to_))
