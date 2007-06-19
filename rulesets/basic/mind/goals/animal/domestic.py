#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2007 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from Vector3D import Vector3D

import re

sowee_pattern = re.compile("[Ss]owee")

from mind.goals.dynamic.DynamicGoal import DynamicGoal

class driven(DynamicGoal):
    def __init__(self, desc="Move away when touched, as by a swineherd"):
        DynamicGoal.__init__(self, trigger="touch", desc=desc)
    def event(self, me, original_op, op):
        distance=distance_to(me.map.get(op.from_).location, me.location)
        destination=Location()
        destination.velocity=distance.unit_vector()
        return Operation("move", Entity(me.id, location=destination))


class summons(DynamicGoal):
    def __init__(self, verb, desc="Come to a stop when commanded to"):
        DynamicGoal.__init__(self, trigger="sound_talk", desc=desc)
    def event(self, me, original_op, op):
        # FIXME Trigger this on interlinguish instead
        # FIXME Use the verb rather than the hardcoded RE
        talk_entity=op[0]
        if hasattr(talk_entity, "say"):
           say=talk_entity.say
           if sowee_pattern.match(say):
              destination=Location()
              destination.velocity=Vector3D(0,0,0)
              return Operation("move", Entity(me.id, location=destination))
