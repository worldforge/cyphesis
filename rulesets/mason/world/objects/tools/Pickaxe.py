#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
#Copyright (C) 2004 Al Riddoch (See the file COPYING for details).

from atlas import *

from world.objects.Thing import Thing

class Pickaxe(Thing):
    """This is a pickaxe for heavy digging and quarrying """
    def cut_operation(self, op):
        ent=Entity(self.id,status=self.status-0.01)
        to_ = op[0].id
        return Message(Operation("set",ent,to=self),Operation("delve",op[0],to=to_))
