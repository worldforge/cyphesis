#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
#return Operation("create",Entity(name='wood',type=['lumber'],location=self.location.parent.location.copy()),to=self)

from atlas import *

import server

class Sword(server.Thing):
    """This is base class for swords, this one just ordinary sword"""
    def cut_operation(self, op):
        to_ = op[1].id
        return Operation("touch",op[1],to=to_,from_=self)
