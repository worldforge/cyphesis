#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
#return Operation("create",Entity(name='wood',type=['lumber'],location=self.location.parent.location.copy()),to=self)

from atlas import *

from world.objects.Thing import Thing

class Axe(Thing):
    """This is base class for axes, this one just ordinary axe"""
    def cut_operation(self, op):
        ent=Entity(self.id,status=self.status-0.01)
        #return Message(Operation("set",ent,to=self),Operation("create",Entity(name='lumber',type=['lumber'],location=self.location.parent.location.copy()),to=self))
        #to_ = self.world.get_object(op[1].id)
        #if not to_:
            #return self.error(op,"To is undefined object")
        to_ = op[0].id
        return Message(Operation("set",ent,to=self),Operation("chop",op[0],to=to_))
