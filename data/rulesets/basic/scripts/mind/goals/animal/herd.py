# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import Operation, Entity
from physics import Point3D
from rules import Location

from mind.goals.dynamic.DynamicGoal import DynamicGoal


class School(DynamicGoal):
    """Move in a shoal with other animals of the same type"""

    def __init__(self, members=[], desc="mirror the movement of other animals like me"):
        DynamicGoal.__init__(self,
                             trigger="sight_move",  # replace with something more appropriate
                             desc=desc)

    def event(self, me, original_op, op):
        # print "school called"
        ent = op[0]
        if ent.id == me.entity.id:
            # print "detecting myself"
            return
        ent = me.map.get(ent.id)
        if ent is None:
            # print "type is none"
            return
        if ent.name != me.name:
            # print "ent.type!=me.entity.type"
            return
        if not ent.parent:
            # print "school.event, ent.parent is None"
            return
        if not me.entity.parent:
            # print "school.event, me.entity.parent is None"
            return
        if me.entity.parent.id != ent.parent.id:
            # print "me.entity.parent.id!=ent.parent.id"
            return
        if type(ent.location.pos) != object:
            # print ent.location.pos
            # print type(ent.location.pos)
            # print "type(ent.location.pos)!=object"
            return
        distance = (ent.location.pos - me.entity.location.pos).mag()
        focus_id = me.get_knowledge('focus', 'hook')
        if focus_id:
            thing = me.map.get(focus_id)
            if thing is None:
                me.remove_knowledge('focus', self.what)
            else:
                if thing.parent.id != me.entity.parent.id:
                    me.remove_knowledge('focus', self.what)
                else:
                    if thing.parent.id == me.entity.id:
                        return
        # ensures that the entity will check only other entities really close to it,
        # thereby reducing the possibility of infinite loops
        if distance < 0.4 and ent.location.velocity:
            print("changing only velocity")
            new_loc = Location(me.entity.parent)
            new_loc.velocity = ent.location.velocity
        if distance > 0.4 and ent.location.velocity:
            print("changing both location and velocity")
            myvel = me.entity.location.velocity.unit_vector()
            evel = ent.location.velocity.unit_vector()
            edir = (ent.location.pos - me.entity.location.pos).unit_vector()
            if myvel and (evel.dot(myvel) > 0.9 or edir.dot(myvel) > 0.9):
                return
            if edir.dot(evel) < 0:
                new_loc = Location(me.entity.parent)
                # replace by rotatez?
                new_loc.velocity = -ent.location.velocity
            else:
                new_loc = Location(me.entity.parent)
                new_loc.velocity = ent.location.velocity
        else:
            print("everything perfect, not doing anything")
            new_loc = ent.location.copy()
            edir = (ent.location.pos - me.entity.location.pos).unit_vector()
            new_loc.pos = new_loc.pos - edir
        return Operation("move", Entity(me.entity.id, location=new_loc))


class flock(DynamicGoal):
    """Move in a flock with other animals of the same type."""

    def __init__(self, members=[], desc="move in flocks with other animals like me"):
        DynamicGoal.__init__(self,
                             trigger="sight_move",
                             desc=desc)

    def event(self, me, original_op, op):
        ent = op[0]
        # This goal currently causes mayhem. Effectively disable it.
        if 1:
            return
        if ent.id == me.entity.id:
            return
        ent = me.map.get(ent.id)
        if ent == None:
            print("not convering on Nothing")
            return
        if ent.type[0] != me.entity.type[0]:
            print("not convering on something not me")
            return
        if type(ent.parent) == type(None):
            print("flock.event, ent.parent is None")
            return
        if type(me.entity.parent) == type(None):
            print("flock.event, me.entity.parent is None")
            return
        if me.entity.parent.id != ent.parent.id:
            print("not convering on something elsewhere")
            return
        if type(ent.location.pos) != Point3D:
            print("position not an Point", type(ent.location.pos))
            return
        edist = (ent.location.pos - me.entity.location.pos)
        if edist.sqr_mag() < 50:
            print("not convering on close enough")
            return
        evel = ent.location.velocity
        if evel and evel.sqr_mag() > 0.1:
            myvel = me.entity.location.velocity
            edir = edist.unit_vector()
            if myvel and myvel.sqr_mag() > 0.1:
                myvel = myvel.unit_vector()
                # If I move in the same direction, then do nothing
                if evel.dot(myvel) > 0.5:
                    print("not convering on moving with")
                    return
                # If I am moving towards them, then do nothing
                if edir.dot(myvel) > 0.5:
                    print("not convering on moving towards them")
                    return
            # If they are coming towards me, then do nothing
            if edir.dot(evel) < - 0.5:
                print("not convering on moving towards me")
                return
            new_loc = Location(me.entity.parent)
            new_loc.velocity = ent.location.velocity
        else:
            new_loc = ent.location.copy()
            edir = (ent.location.pos - me.entity.location.pos).unit_vector()
            new_loc.pos = new_loc.pos - edir
        print("converging")
        return Operation("move", Entity(me.entity.id, location=new_loc))


class herd(DynamicGoal):
    """Move in a herd with other animals of the same type."""

    def __init__(self, members=[], desc="move in herds with other animals like me"):
        DynamicGoal.__init__(self,
                             trigger="sight_move",
                             desc=desc)
        for m in members:
            self.herd_members[m] = 51
        self.herd_members = {}

    def event(self, me, original_op, op):
        ent = op[0]
        if ent.id == me.entity.id:
            return
        ent = me.map.get(ent.id)
        if ent is None:
            return
        if ent.type[0] != me.entity.type[0]:
            return
        if me.entity.parent.id != ent.parent.id:
            return
        try:
            val = self.herd_members[ent.id]
        except KeyError:
            val = 0
        if type(ent.location.pos) != Point3D:
            return
        if me.entity.location.pos.distance(ent.location.pos) < 6:
            val = val + 1
            self.herd_members[ent.id] = val
            return
        # If we have not seen this one before 50 times, then it is not yet
        # really a member of the herd
        if not val > 50:
            return
        val = val + 1
        self.herd_members[ent.id] = val
        if ent.location.velocity:
            myvel = me.entity.location.velocity.unit_vector()
            evel = ent.location.velocity.unit_vector()
            edir = (ent.location.pos - me.entity.location.pos).unit_vector()
            # If I am moving towards them, or in the same direction, then do nothing
            if myvel and (evel.dot(myvel) > 0.5 or edir.dot(myvel) > 0.5):
                return
            # If they are coming towards me, then do nothing
            if edir.dot(evel) < - 0.5:
                return
            new_loc = Location(me.entity.parent)
            new_loc.velocity = ent.location.velocity
        else:
            new_loc = ent.location.copy()
            edir = (ent.location.pos - me.entity.location.pos).unit_vector()
            new_loc.pos = new_loc.pos - edir
        return Operation("move", Entity(me.entity.id, location=new_loc))
