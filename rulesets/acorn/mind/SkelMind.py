from atlas import *
from whrandom import *
from world.objects.Thing import Thing

BaseMind = Thing

class SkeletonMind(BaseMind):
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
    def sight_move_operation(self, original_op, op):
        self.map.update(op[0])
        if op[0].id==self.id: return
        #target=op[0].location.copy()
        other = self.map.add(op[0])
        if other.location.parent.id!=self.location.parent.id: return
        if hasattr(op.from_, "type") and op.from_.type[0] not in ['pig', 'farmer', 'guard']: return
        destination=op[0].location.coordinates
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
