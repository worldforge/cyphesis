#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2009 Amey Parulekar (See the file COPYING for details).

from atlas import *

import server

class Sieve(server.Thing):
    """This is a sieve for light earthworm-ing"""
    def cut_operation(self, op):
        ent=Entity(self.id,status=self.status-0.01)
        to_ = op[0].id
        return Oplist(Operation("set",ent,to=self),Operation("sieve",op[0],to=to_))

