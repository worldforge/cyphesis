#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from world.objects.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 1,1.5,1.5
# bmedian = 0.5,1.0,1.5
# offset = SW corner = -0.5,-0.5,0

class Stall_se(Thing):
    def setup_operation(self, op):
        ret = Message()
        # South counter
	loc = Location(self, Vector3D(-0.5,-0.5,0))
        loc.bbox = Vector3D(1.5,0.25,0.5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # North back wall
	loc = Location(self, Vector3D(-0.5,1.3,0))
        loc.bbox = Vector3D(1.5,0.1,1.5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        return ret

class Stall_sw(Thing):
    def setup_operation(self, op):
        ret = Message()
        # West counter
	loc = Location(self, Vector3D(-0.5,-0.5,0))
        loc.bbox = Vector3D(0.25,1.5,0.5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        # North back wall
	loc = Location(self, Vector3D(1.3,-0.5,0))
        loc.bbox = Vector3D(0.1,1.5,1.5)
        ret.append(Operation("create",Entity(name='wall',parents=['wall'],location=loc),to=self))
        return ret


class Mstall_bakery_2_se(Stall_se): pass
class Mstall_beer_1_se(Stall_se): pass
class Mstall_blacksmith_1_sw(Stall_sw): pass
class Mstall_books_2_sw(Stall_sw): pass
class Mstall_cheese_2_se(Stall_se): pass
class Mstall_fish_1_se(Stall_se): pass
class Mstall_freshmeat_1_se(Stall_se): pass
class Mstall_fruits_2_se(Stall_se): pass
class Mstall_healer_1_sw(Stall_sw): pass
class Mstall_household_1_sw(Stall_sw): pass
class Mstall_jewels_1_sw(Stall_sw): pass
class Mstall_magic_items_1_sw(Stall_sw): pass
class Mstall_milk_2_se(Stall_se): pass
class Mstall_seamstress_1_sw(Stall_sw): pass
class Mstall_vegetables_1_se(Stall_se): pass
class Mstall_wine_1_se(Stall_se): pass
