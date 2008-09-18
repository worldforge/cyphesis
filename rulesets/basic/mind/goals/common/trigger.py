#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2007 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from Vector3D import Vector3D

try:
  from random import *
except ImportError:
  from whrandom import *

from mind.goals.dynamic.DynamicGoal import DynamicGoal

class chase(DynamicGoal):
    """Chase after something that has moved."""
    def __init__(self, kinds, desc="Chase after things."):
        DynamicGoal.__init__(self, trigger="sight_move", desc=desc)
        self.kinds = kinds
    def event(self, me, original_op, op):
        # FIXME Now that this code is trigger goal, has this update been done?
        other = me.map.update(op[0], op.getSeconds())
        if other.id==me.id: return
        #target=op[0].location.copy()
        if other.location.parent.id!=me.location.parent.id: return
        if hasattr(other, "type") and other.type[0] not in self.kinds: return
        destination=other.location.coordinates
        distance=destination.distance(me.location.coordinates)
        if distance<1: return
        # CHeat, random chance that it ignores movement
        if uniform(0, 30/distance)<1: return
        target=Location(me.location.parent)
        velocity=destination-me.location.coordinates
        if velocity.mag()==0: return
        target.velocity=velocity.unit_vector()
        target.coordinates=destination
        return Operation("move", Entity(me.id, location=target))

