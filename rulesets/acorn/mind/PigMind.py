from atlas import *
from world.physics.Vector3D import Vector3D
from mind.NPCMind import NPCMind

class PigMind(NPCMind):
    def __init__(self, **kw):
        self.base_init(kw)
    def touch_operation(self, op):
        source=op.from_.location.coordinates
        destination=Location(self.location.parent)
        destination.velocity=source.unit_vector_to_another_vector(self.get_xyz())
        return Operation("move", Entity(self.id, location=destination))
    def sound_talk_operation(self, original_op, op):
        talk_entity=op[0]
        if hasattr(talk_entity, "say"):
           say=talk_entity.say
           if say=="Sowee" or say=="sowee":
              if op.from_.location.parent!=self.location.parent:
                  return
              source=op.from_.location.coordinates
              distance=(self.location.coordinates-source).mag()
              if distance>10:
                  return
              destination=Location(self.location.parent,velocity=Vector3D(0,0,0))
              return Operation("move", Entity(self.id, location=destination))
        return NPCMind.sound_talk_operation(self, original_op, op)
