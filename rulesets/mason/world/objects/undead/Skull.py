from atlas import *

from world.objects.Thing import Thing
from misc import set_kw

class Skull(Thing):
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
        set_kw(self,kw,"mass",1.0)
