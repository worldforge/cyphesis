from atlas import *
from Vector3D import Vector3D
from mind.NPCMind import NPCMind

import re

sowee_pattern = re.compile("[Ss]owee")

class PigMind(NPCMind):
    def touch_operation(self, op):
        source=op.from_.get_xyz()
        destination=Location()
        destination.velocity=source.unit_vector_to_another_vector(self.get_xyz())
        return Operation("move", Entity(self.id, location=destination))
    def sound_talk_operation(self, original_op, op):
        talk_entity=op[0]
        if hasattr(talk_entity, "say"):
           say=talk_entity.say
           if sowee_pattern.match(say):
              destination=Location()
              destination.velocity=Vector3D(0,0,0)
              return Operation("move", Entity(self.id, location=destination))
        return NPCMind.sound_talk_operation(self, original_op, op)
