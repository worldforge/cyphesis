from atlas import *

from world.objects.Thing import Thing
from common.misc import set_kw

class Skull(Thing):
    def __init__(self, **kw):
        self.base_init(kw)
        set_kw(self,kw,"weight",1.0)
