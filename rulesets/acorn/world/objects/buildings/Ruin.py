#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from cyphesis.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 
# bmedian = 
# offset = SW corner = 

class Shop_itemshop_deco_agril_1_sesw(Thing):
    def setup_operation(self, op):
        ret = Message()
        # South wall
	loc = Location(self, Vector3D(0,0,0))
        loc.bbox = Vector3D(2,0.5,5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        return ret
