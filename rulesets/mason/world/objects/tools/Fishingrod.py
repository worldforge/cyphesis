#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2009 Amey Parulekar (See the file COPYING for details).

from atlas import *

import server

class Fishingrod(server.Thing):
    """This is tool for fishing in the ocean"""
    def sow_operation(self, op):
        ent=Entity(self.id,status=self.status-0.01)
        to_ = op[0].id
        return Oplist(Operation("set",ent,to=self),Operation("fishing",op[0],to=to_))
