#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
#return Operation("create",Entity(name='wood',type=['lumber'],location=self.location.parent.location.copy()),to=self)

from Point3D import Point3D
from atlas import *

from cyphesis.Thing import Thing

class Gallows(Thing):
    """This is the gallows, which hangs villagers"""
    def hang_operation(self, op):
        to_ = op[0].id
        reply = Message()
        loc_ = Location(self.id, Point3D(2.5,2.5,3))
        reply.append(Operation("move",Entity(to_,location=loc_,mode="hanging"),to=to_))
        reply.append(Operation("reveal",Entity(to_),to=to_))
        # Send op to target which makes it reveal its true nature, and then die
        return reply
