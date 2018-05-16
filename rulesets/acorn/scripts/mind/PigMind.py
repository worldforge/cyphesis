from atlas import *
from Vector3D import Vector3D
from mind.NPCMind import NPCMind

import re

sowee_pattern = re.compile("[Ss]owee")

class PigMind(NPCMind):
    def touch_operation(self, op):
        distance=distance_to(op.from_.location, self.location)
        destination=Location()
        destination.velocity=distance.unit_vector()
        return Operation("move", Entity(self.id, location=destination))
    def sound_talk_operation(self, original_op, op):
        talk_entity=op[0]
        if hasattr(talk_entity, "say"):
           say=talk_entity.say
           if sowee_pattern.match(say):
              if op.from_.location.parent!=self.location.parent:
                  return
              source=op.from_.location.coordinates
              distance=(self.location.coordinates-source).mag()
              if distance>10:
                  return
              destination=Location(self.location.parent)
              destination.velocity=Vector3D(0,0,0)
              return Operation("move", Entity(self.id, location=destination))
        return NPCMind.sound_talk_operation(self, original_op, op)
