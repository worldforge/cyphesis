#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

import server

class Cleaver(server.Thing):
    """This is base class for axes, this one just ordinary axe"""
    def cut_operation(self, op):
        to_ = op[0].id
        if not to_:
            return self.error(op,"To is undefined object")
        return Oplist(Operation("chop",op[0],Entity(op.from_),to=to_))
