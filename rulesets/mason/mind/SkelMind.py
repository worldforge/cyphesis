from atlas import *
from whrandom import *
from world.objects.Thing import Thing

BaseMind = Thing

class SkeletonMind(BaseMind):
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
    def sight_move_operation(self, original_op, op):
        self.map.update(op[0])
        other = self.map.update(op[0])
        if other.id==self.id: return
        #target=op[0].location.copy()
        if other.location.parent.id!=self.location.parent.id: return
        if hasattr(other, "type") and other.type[0] not in ['pig', 'farmer', 'guard', 'settler']: return
        destination=other.location.coordinates
        distance=destination.distance(self.location.coordinates)
        if distance<1: return
        # CHeat, random chance that it ignores movement
        if uniform(0, 30/distance)<1: return
        target=Location(self.location.parent)
        velocity=destination-self.location.coordinates
        if velocity.mag()==0: return
        target.velocity=velocity.unit_vector()
        target.coordinates=destination
        return Operation("move", Entity(self.id, location=target))
