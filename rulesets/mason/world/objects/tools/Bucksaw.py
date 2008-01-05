#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005 Al Riddoch (See the file COPYING for details).

from atlas import *

from cyphesis.Thing import Thing

class Bucksaw(Thing):
    """This is a large saw for cutting logs"""
    def cut_operation(self, op):
        print "Bucksaw.cur_operation() Don't call this."
        to_ = op[0].id
        if not to_:
            return self.error(op,"To is undefined object")
        return Message(Operation("saw",op[0],Entity(op.from_),to=to_))
