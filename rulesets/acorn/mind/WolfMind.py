from atlas import *
from mind.NPCMind import NPCMind

class WolfMind(NPCMind):
    def touch_operation(self, op):
        return Operation("talk", Entity(say="Grrrr"))
