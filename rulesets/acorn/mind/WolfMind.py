from atlas import *
from mind.NPCMind import NPCMind

class WolfMind(NPCMind):
    def __init__(self, **kw):
        self.base_init(kw)
    def touch_operation(self, op):
        return Operation("talk", Entity(say="Grrrr"))
