#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

from cyphesis.Thing import Thing
from misc import set_kw

class Cleaver(Thing):
    """This is base class for axes, this one just ordinary axe"""
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
        set_kw(self,kw,"mass",1.5)
    def cut_operation(self, op):
        to_ = op[1].id
        if not to_:
            return self.error(op,"To is undefined object")
        return Oplist(Operation("chop",op[1],Entity(op.from_.id),to=to_))
