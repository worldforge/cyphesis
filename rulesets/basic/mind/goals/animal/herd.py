#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *
from types import *
from common import const

if const.server_python:
    from world.physics.Vector3D import Vector3D
else:
    from Vector3D import Vector3D

from mind.goals.dynamic.DynamicGoal import DynamicGoal

class herd(DynamicGoal):
    def __init__(self, members=[], desc="move in herds with other animals like me"):
        DynamicGoal.__init__(self,
                             trigger="sight_move",
                             desc=desc)
        for m in members:
            self.herd_members[m] = 51
        self.herd_members={}
    def event(self, me, original_op, op):
        ent=op[0]
        if ent.id==me.id: return
        ent=me.map.get(ent.id)
        if ent==None: return
        if ent.type[0]!=me.type[0]: return
        if me.location.parent.id!=ent.location.parent.id: return
        try:
            val = self.herd_members[ent.id]
        except KeyError:
            val=0
        if type(ent.location.coordinates)!=InstanceType:
            return
        if me.location.coordinates.distance(ent.location.coordinates)<6:
            val = val+1
            self.herd_members[ent.id]=val
            return
        #If we have not seen this one before 50 times, then it is not yet
	#really a member of the herd
        if not val > 50: return
        val = val+1
        self.herd_members[ent.id]=val
        if ent.location.velocity:
            myvel=me.location.velocity.unit_vector()
            evel=ent.location.velocity.unit_vector()
            edir=(ent.location.coordinates-me.location.coordinates).unit_vector()
            #If I am moving towards them, or in the same direction, then do nothing
            if myvel and (evel.dot(myvel) > 0.5 or edir.dot(myvel) > 0.5):
                return
            #If they are coming towards me, then do nothing
            if edir.dot(evel) < - 0.5:
                return
            new_loc=Location(me.location.parent)
            new_loc.velocity=ent.location.velocity
        else:
            new_loc=ent.location.copy()
            edir=(ent.location.coordinates-me.location.coordinates).unit_vector()
            new_loc.coordinates=new_loc.coordinates-edir
        return Operation("move", Entity(me.id, location=new_loc))
