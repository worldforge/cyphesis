#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
#return Operation("create",Entity(name='wood',type=['lumber'],location=self.location.parent.location.copy()),to=self)

from atlas import *

from world.objects.Thing import Thing
from misc import set_kw

class Sword(Thing):
    """This is base class for swords, this one just ordinary sword"""
    def cut_operation(self, op):
        #to_ = self.world.get_object(op[1].id)
        #if not to_:
            #return self.error(op,"To is undefined object")
        to_ = op[1].id
        return Operation("touch",op[1],to=to_,from_=self)
