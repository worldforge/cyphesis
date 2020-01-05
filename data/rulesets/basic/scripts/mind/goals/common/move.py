# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).
import math
import types

import entity_filter
from atlas import Operation, Entity, Oplist
from common import const
from physics import Point3D, Vector3D, distance_to, square_horizontal_distance, square_distance
from rules import Location, isLocation

from mind.goals.common.misc_goal import *
from mind.Goal import Goal


############################ MOVE ME ####################################

class MoveMe(Goal):
    """Move me to a certain place.
    'radius' specifies how close to the location we accept.
    """

    def __init__(self, location, radius=0.5, speed=0.2):
        Goal.__init__(self, "move me to certain place",
                      self.am_i_at_loc,
                      [self.move_to_loc],
                      self.is_reachable)
        self.location = location
        self.speed = speed
        self.radius = radius
        self.vars = ["location", "speed", "radius"]
        self.squared_radius = radius * radius

    def get_location_instance(self, me):
        if isinstance(self.location, types.LambdaType) or isinstance(self.location, types.FunctionType) or isinstance(self.location, types.MethodType):
            # print "Lambda location"
            return self.location(me)
        if type(self.location) == str:
            # print "String location"
            return me.get_knowledge("location", self.location)
        return self.location

    def am_i_at_loc(self, me):
        location = self.get_location_instance(me)
        if me.entity.location is None:
            return True
        if not location:
            # print "No location"
            return True
        square_distance = square_horizontal_distance(me.entity.location, location)
        if square_distance and square_distance <= self.squared_radius:
            # print("We are there, distance %s" % math.sqrt(square_distance))
            return True
        else:
            # print("We are not there, distance %s" % math.sqrt(square_distance))
            return False

    def move_to_loc(self, me):
        location = self.get_location_instance(me)
        if not location:
            # print "Can't move - no location"
            return
        if not location.parent:
            return
        me.set_speed(self.speed)
        me.set_destination(location.pos, self.radius, location.parent.id)
        refresh_result = me.refresh_path()
        # If result is 0 it means that we're already there
        if refresh_result == 0:
            return
        # If result is below zero it means that we couldn't find a path yet.
        # This can be because we haven't mapped all areas yet; if so one should check with
        # me.unawareTilesCount
        if refresh_result < 0:
            print("Could not find any path, result %s" % refresh_result)
            return

        # Return True to signal that this goal is complete now.
        return True

    """ Checks that the movement goal is reachable. This will return true if the goal currently can't be reached, but there are still
    unaware tiles."""

    def is_reachable(self, me):
        location = self.get_location_instance(me)
        if not location:
            # We have no location, so no valid goal
            return False

        path_result = me.pathResult

        # print("pathResult " + str(pathResult))
        if 0 > path_result > -7:
            # print("unawareTilesCount " + str(me.unawareTilesCount))

            if me.unawareTilesCount == 0:
                return False
        elif path_result == 0 and not self.am_i_at_loc(me):
            # If there are no more segments in the path, but we haven't yet reached the destination then something is preventing us from reaching it
            return False

        return True


############################ MOVE ME AREA ####################################

class MoveMeArea(Goal):
    """Move me to a certain area."""

    def __init__(self, location, range=30):
        Goal.__init__(self, "move me to certain area",
                      self.am_i_in_area,
                      [MoveMe(location, range), self.latch_loc],
                      self.is_reachable)
        self.location = location
        self.range = range
        self.square_range = range * range
        self.arrived = 0
        self.vars = ["location", "range", "arrived"]

    def get_location_instance(self, me):
        # FIXME Duplicate of method from move_me() goal
        location_ = self.location
        if isinstance(location_, types.LambdaType):
            # print "Lambda location"
            location_ = location_(me)
        if type(location_) == str:
            # print "String location"
            location_ = me.get_knowledge("location", location_)
        if not location_:
            # print "Unknown location"
            return None
        return location_

    def am_i_in_area(self, me):
        location = self.get_location_instance(me)
        if not location:
            # print "No location"
            return 0
        if self.arrived:
            # print "Already arrived at location"
            square_dist = square_distance(me.entity.location, location)
            if square_dist > self.square_range:
                self.arrived = 0
                # print "Moved away"
                return 0
            else:
                # print "Still here", square_dist, self.square_range
                return 1
        # print "I am not there"
        return 0

    def latch_loc(self, me):
        # print "Latching at location"
        self.arrived = 1

    """ Checks that the movement goal is reachable. This will return true if the goal currently can't be reached, but there are still
    unaware tiles."""

    def is_reachable(self, me):
        path_result = me.pathResult

        # print("pathResult " + str(pathResult))
        if 0 > path_result > -7:
            # print("unawareTilesCount " + str(me.unawareTilesCount))

            if me.unawareTilesCount == 0:
                return False
        elif path_result == 0 and not self.am_i_in_area(me):
            # If there are no more segments in the path, but we haven't yet reached the destination then something is preventing us from reaching it
            return False

        return True


############################ MOVE ME PLACE ####################################

class MoveMePlace(MoveMe):
    """Move me to a place by name."""

    def __init__(self, what):
        Goal.__init__(self, "move me to a place where I can get something",
                      self.am_i_at_loc,
                      [self.move_to_loc])
        self.what = what
        self.vars = ["what"]

    def get_location_instance(self, me):
        location = me.get_knowledge("place", self.what)
        if type(location) == str:
            location = me.get_knowledge("location", location)
        if not location:
            return None
        return location


############################ MOVE THING ####################################

class MoveIt(Goal):
    """Move something to a place."""

    def __init__(self, what, location, speed=0):
        Goal.__init__(self, "move this to certain place",
                      self.is_it_at_loc,
                      [self.move_it_to_loc])
        self.what = what
        self.speed = speed
        self.location = location
        self.wait = 0
        self.vars = ["what", "location", "speed", "wait"]

    def is_it_at_loc(self, me):
        # CHEAT!: cludge
        if self.wait > 0:
            return 0
        if type(self.location) == str:
            self.location = me.get_knowledge("location", self.location)
        if not isLocation(self.location):
            self.location = Location(self.location, Point3D(0.0, 0.0, 0.0))
        if type(self.what) == str:
            if (self.what in me.things) == 0: return 1
            what = me.things[self.what][0]
        if what.location.parent.id != self.location.parent.id: return 0
        return what.location.pos.distance(self.location.pos) < 1.5

    def move_it_to_loc(self, me):
        if self.wait > 0:
            self.wait = self.wait - 1
            return
        if type(self.location) == str:
            self.location = me.get_knowledge("location", self.location)
        elif not isLocation(self.location):
            self.location = Location(self.location, Point3D(0.0, 0.0, 0.0))
        if type(self.what) == str:
            if (self.what in me.things) == 0:
                return
            what = me.things[self.what][0]
        if self.speed == 0 or what.location.parent.id != self.location.parent.id:
            return Operation("move", Entity(what.id, location=self.location))
        iloc = what.location.copy()
        vel = what.location.pos.unit_vector_to(self.location.pos)
        iloc.velocity = vel * self.speed
        self.location.velocity = Vector3D(0.0, 0.0, 0.0)
        mOp1 = Operation("move", Entity(what.id, location=iloc))
        mOp2 = Operation("move", Entity(what.id, location=self.location))
        time = ((self.location.pos - what.location.pos).mag() / self.speed)
        self.wait = (time / const.basic_tick) + 1
        mOp2.set_future_seconds(time)
        return Oplist(mOp1, mOp2)


############################ MOVE THING FROM ME ####################################

class MoveItOutOfMe(Goal):
    """Put something down."""

    def __init__(self, what):
        Goal.__init__(self, "move this thing from my inventory and disown",
                      self.is_it_not_with_me,
                      [self.drop_it])
        self.what = what
        self.what_filter = entity_filter.Filter(what)

    def is_it_not_with_me(self, me):
        things = me.match_entities(self.what_filter, me.entity.contains)
        return len(things) == 0

    def drop_it(self, me):
        things = me.match_entities(self.what_filter, me.entity.contains)
        if things > 0:
            me.remove_thing(things[0])
            return Operation("move", Entity(things[0].id, location=me.entity.location))
        else:
            return


############################ MOVE ME TO THING ##################################

class MoveMeToPossession(Goal):
    """Move me to the same place as something I own."""

    def __init__(self, what):
        Goal.__init__(self, "move me to this thing",
                      self.am_i_at_it,
                      [self.move_me_to_it])
        self.what = what
        self.vars = ["what"]

    def am_i_at_it(self, me):
        what = self.what
        if type(what) == str:
            if (what in me.things) == 0: return 0
            what = me.things[what][0]
        distance = square_horizontal_distance(me.entity.location, what.location)
        if distance and distance < 4:  # 2 * 2
            return 1
        else:
            return 0

    def move_me_to_it(self, me):
        what = self.what
        if type(what) == str:
            if (what in me.things) == 0: return
            what = me.things[what][0]
        target = what.location.copy()
        if target.parent.id == me.entity.location.parent.id:
            target.velocity = me.entity.location.pos.unit_vector_to(target.pos)
            target.rotation = target.velocity
            return Operation("move", Entity(me.entity.id, location=target))


class MoveMeToFocus(Goal):
    """Move me to something I am interested in."""

    def __init__(self, what="", radius=0.5, speed=0.2):
        Goal.__init__(self, "move me to the current focus for '%s'" % what,
                      None,
                      [MoveMe(location=self.get_location, radius=radius, speed=speed)])
        self.what = what
        self.vars = ["what"]

    def get_location(self, me):
        id = me.get_knowledge('focus', self.what)
        if id is None:
            return None
        thing = me.map.get(id)
        if thing is None:
            me.remove_knowledge('focus', self.what)
            return None
        return thing.location


# class move_me_to_focus(Goal):
#     """Move me to something I am interested in."""
#
#     def __init__(self, what, distance=2):
#         Goal.__init__(self, "move me to this thing",
#                       self.am_i_at_it,
#                       [self.move_me_to_it])
#         if type(what) == list:
#             self.what = what
#         else:
#             self.what = [what]
#         # How close we need to get to the thing.
#         self.distance = distance
#         self.vars = ["what", "distance"]
#
#     def am_i_at_it(self, me):
#         for what in self.what:
#             id = me.get_knowledge('focus', what)
#             if id == None: continue
#             thing = me.map.get(id)
#             if thing == None:
#                 me.remove_knowledge('focus', what)
#                 continue
#
#             # Only move to the edge of the entity, since else we'll just collide with it.
#             # TODO: Make this check better, taking into account the real collision volume, rotated and all.
#             bbox_size = thing.location.bbox.square_horizontal_bounding_radius()
#             # TODO: Add a check for solid and non solid entities.
#             # When moving to a non solid entity, we should try to get at its center.
#             if square_horizontal_distance(me.entity.location, thing.location) < ((self.distance * self.distance) + bbox_size):
#                 return 1
#         return 0
#
#     def move_me_to_it(self, me):
#         for what in self.what:
#             id = me.get_knowledge('focus', what)
#             if id == None: continue
#             thing = me.map.get(id)
#             if thing == None:
#                 me.remove_knowledge('focus', what)
#                 return
#             target = thing.location.copy()
#             if target.parent.id == me.entity.location.parent.id:
#                 target.velocity = me.entity.location.pos.unit_vector_to(target.pos)
#                 return Operation("move", Entity(me.entity.id, location=target))


class MoveMeNearFocus(Goal):
    """Move me to something I am interested in, first really close, and the allowing movement within a certain radius."""

    def __init__(self, what, distance=2, allowed_movement_radius=10):
        Goal.__init__(self, "move me near this thing",
                      self.am_i_at_it,
                      [self.move_me_to_it])
        if type(what) == list:
            self.what = what
        else:
            self.what = [what]
        # The radius within which we're allowed movement.
        self.allowed_movement_radius = allowed_movement_radius
        # How close we need to get to the thing.
        self.distance = distance
        self.vars = ["what", "distance", "allowed_movement_radius"]
        # Keeps track of if we're close enough to a thing to consider us near it.
        # If this is true, i.e. we're near a thing, we consider this goal fulfilled as long as we're within the allowed_movement_radius
        self.is_close_to_thing = False

    def am_i_at_it(self, me):
        for what in self.what:
            id = me.get_knowledge('focus', what)
            if id is None: continue
            thing = me.map.get(id)
            if thing is None:
                me.remove_knowledge('focus', what)
                continue

            # Only move to the edge of the entity, since else we'll just collide with it.
            # TODO: Make this check better, taking into account the real collision volume, rotated and all.
            bbox_size = thing.location.bbox.square_horizontal_bounding_radius()
            # TODO: Add a check for solid and non solid entities.
            # When moving to a non solid entity, we should try to get at its center.
            squared_distance_to_thing = square_horizontal_distance(me.entity.location, thing.location)
            if not squared_distance_to_thing:
                return False
            if squared_distance_to_thing < ((self.distance * self.distance) + bbox_size):
                self.is_close_to_thing = True
                return True
            # If we've already moved close to the thing, we are allowed movement within a certain movement radius
            if self.is_close_to_thing:
                if squared_distance_to_thing < (
                        (self.allowed_movement_radius * self.allowed_movement_radius) + bbox_size):
                    return True
        self.is_close_to_thing = False
        return False

    def move_me_to_it(self, me):
        for what in self.what:
            id = me.get_knowledge('focus', what)
            if id is None:
                continue
            thing = me.map.get(id)
            if thing is None:
                me.remove_knowledge('focus', what)
                return
            target = thing.location.copy()
            if target.parent.id == me.entity.location.parent.id:
                target.velocity = me.entity.location.pos.unit_vector_to(target.pos)
                return Operation("move", Entity(me.entity.id, location=target))


############################ MOVE THING TO ME ####################################

class PickUpPossession(Goal):
    """Pick up something I own."""

    def __init__(self, what):
        Goal.__init__(self, "move this thing to my inventory (class)",
                      self.is_it_with_me,
                      [MoveMeToPossession(what),
                       self.pick_it_up])
        self.what = what
        self.vars = ["what"]

    def is_it_with_me(self, me):
        # CHEAT!: cludge
        what = self.what
        if type(what) == str:
            if (self.what in me.things) == 0: return 0
            what = me.things[self.what][0]
        if what.location.parent.id != me.entity.id:
            if what.location.parent.id != me.entity.location.parent.id:
                me.remove_thing(what.id)
                me.map.delete(what.id)
        return what.location.parent.id == me.entity.id

    def pick_it_up(self, me):
        what = self.what
        if type(what) == str:
            if (self.what in me.things) == 0: return 0
            what = me.things[self.what][0]
        return Operation("move", Entity(id, location=Location(me, Point3D(0, 0, 0))))


class PickUpFocus(Goal):
    """Pick up something I am interested in."""

    def __init__(self, what):
        Goal.__init__(self, "move this thing to my inventory (class)",
                      self.is_it_with_me,
                      [MoveMeToFocus(what),
                       self.pick_it_up])
        if type(what) == list:
            self.what = what
        else:
            self.what = [what]
        self.vars = ["what"]

    def is_it_with_me(self, me):
        # CHEAT!: cludge
        for what in self.what:
            id = me.get_knowledge('focus', what)
            if id == None: continue
            thing = me.map.get(id)
            if thing == None:
                me.remove_knowledge('focus', what)
                continue
            # If its not not near us on the ground, forget about it.
            if thing.location.parent.id != me.entity.location.parent.id:
                me.remove_knowledge('focus', what)
                continue
            if thing.location.parent.id != me.entity.id:
                return 0
        return 1

    def pick_it_up(self, me):
        for what in self.what:
            id = me.get_knowledge('focus', what)
            if id == None: continue
            thing = me.map.get(id)
            if thing == None:
                me.remove_knowledge('focus', what)
                continue
            if thing.location.parent.id != me.entity.id:
                return Operation("move", Entity(id, location=Location(me, Point3D(0, 0, 0))))


############################ WANDER ####################################

class Wander(Goal):
    """Move in a non-specific way."""

    def __init__(self, extragoal=None):
        Goal.__init__(self, "wander randomly", false,
                      [MoveMe(None),
                       extragoal,
                       self.do_wandering])

    def do_wandering(self, me):
        loc = me.entity.location.copy()
        loc.pos = Point3D([c + uniform(-5, 5) for c in loc.pos])
        self.subgoals[0].location = loc


############################ WANDER & SEARCH ############################

class Search(Goal):
    """Move in a non-specific way looking for something."""

    def __init__(self, what):
        Goal.__init__(self, "search for a thing",
                      self.do_I_have,
                      [Wander(),
                       SpotSomething(what, 30)])
        # Long range for testing only
        self.what = what
        self.vars = ["what"]

    def do_I_have(self, me):
        return (self.what in me.things) == 1


############################ PURSUIT ####################################

class Pursuit(Goal):
    """avoid or hunt something at range"""

    def __init__(self, desc="pursue something", what="", range=0, direction=1):
        Goal.__init__(self,
                      "pursue something",
                      self.not_visible,
                      [self.run])

        if isinstance(what, str):
            self.what = what
        elif isinstance(what, list) and len(what) > 0:
            # Try to use the first element as a query.
            # Queries should never really be passed as a list
            self.what = str(what[0])
        else:
            self.what = str(what)
        self.filter = entity_filter.Filter(self.what)
        self.range = range
        self.direction = direction
        self.vars = ["what", "range", "direction"]

    def not_visible(self, me):
        # print self.__class__.__name__,me.mem.recall_place(me.entity.location,self.range,self.what)
        return not me.mem.recall_place(me.entity.location, self.range, self.filter)

    def run(self, me):
        lst_of_what = me.mem.recall_place(me.entity.location, self.range, self.filter)
        if not lst_of_what or len(lst_of_what) == 0: return
        dist_vect = distance_to(me.entity.location, lst_of_what[0].location).unit_vector()
        multiply = 1.0 * self.direction * const.basic_tick
        loc = Location(me.entity.location.parent)
        loc.pos = me.entity.location.pos + (dist_vect * multiply)
        ent = Entity(me.entity.id, location=loc)
        return Operation("move", ent)


############################ AVOID ####################################

class Avoid(Pursuit):
    """avoid something at range"""

    def __init__(self, what='', range=0):
        Pursuit.__init__(self, desc="avoid something", what=what, range=range, direction=-1)


################################ HUNT ################################

class Hunt(Pursuit):
    """hunt something at range"""

    def __init__(self, what, range):
        Pursuit.__init__(self, "hunt something", what, range, 1)


class HuntFor(Goal):
    """hunt something at range"""

    def __init__(self, what="", range=0, proximity=5):
        Goal.__init__(self, "hunt for something",
                      self.in_range,
                      [Pursuit(what=what, range=range)])
        self.what = what
        self.range = range
        self.proximity = proximity
        self.square_proximity = proximity * proximity
        self.vars = ["what", "range", "proximity"]

    def in_range(self, me):
        id = me.get_knowledge('focus', self.what)
        if id is None:
            return
        thing = me.map.get(id)
        if thing is None:
            return
        square_dist = square_distance(me.entity.location, thing.location)
        return square_dist and square_dist < self.square_proximity


################################ PATROL ##############################

class Patrol(Goal):
    """Move around an area defined by some waypoints."""

    def __init__(self, locations, extragoal=None):
        Goal.__init__(self, "patrol an area",
                      false,
                      [MoveMe(locations[0]),
                       extragoal,
                       self.increment],
                      self.check_move_valid)
        self.list = locations
        self.stage = 0
        self.count = len(locations)
        self.vars = ["stage", "list"]

    def check_move_valid(self, me):
        """ Checks that the movement goal is reachable; if not we should move on to the next patrol goal """
        return self.subgoals[0].is_valid(me)

    def increment(self, me):
        self.stage = self.stage + 1
        if self.stage >= self.count:
            self.stage = 0
        self.subgoals[0].location = self.list[self.stage]
        # print "Moved to next patrol goal: " + str(self.subgoals[0].location)


############################## ACCOMPANY ##############################

class Accompany(Goal):
    """Move around staying close to someone."""

    def __init__(self, who):
        Goal.__init__(self, "stay with someone",
                      self.am_i_with,
                      [SpotSomething(who),
                       self.follow])
        self.who = who
        self.vars = ["who"]

    def am_i_with(self, me):
        id = me.get_knowledge('focus', self.who)
        who = me.map.get(str(id))
        if who is None:
            return 0

        dist = distance_to(me.entity.location, who.location)
        # Are we further than 3 metres away
        if dist.sqr_mag() > 25:
            # print "We are far away", dist
            if me.entity.location.velocity.is_valid() and me.entity.location.velocity.dot(dist) > 0.5:
                # print "We moving towards them already"
                return 1
            return 0
        else:
            # print "We are close", dist
            if me.entity.location.velocity.is_valid() and me.entity.location.velocity.dot(dist) < 0.5:
                # print "We going away from them"
                return 0
            return 1

    def follow(self, me):
        id = me.get_knowledge('focus', self.who)
        who = me.map.get(str(id))
        if who == None:
            self.irrelevant = 1
            return
        dist = distance_to(me.entity.location, who.location)
        target = Location(me.entity.location.parent)
        square_dist = dist.sqr_mag()
        if square_dist > 64:
            # print "We must be far far away - run"
            target.velocity = dist.unit_vector() * 3
        elif square_dist > 25:
            # print "We must be far away - walk"
            target.velocity = dist.unit_vector()
        else:
            # print "We must be close - stop"
            target.velocity = Vector3D(0, 0, 0)
        return Operation("move", Entity(me.entity.id, location=target))


############################ ROAM ####################################

class Roam(Goal):
    """Move in a non-specific way within one or many locations."""

    def __init__(self, radius, locations, extragoal=None):
        Goal.__init__(self, "roam randomly", false,
                      [self.check_move_valid,
                       MoveMe(None),
                       goal_create(extragoal),
                       self.do_roaming])
        self.list = locations
        self.radius = radius
        self.count = len(locations)
        self.vars = ["radius", "list"]

    def do_roaming(self, me):
        move_me_goal = self.subgoals[1]
        # We need to set a new direction if we've either haven't set one, or if we've arrived.
        if move_me_goal.location == None or move_me_goal.fulfilled(me) == True or move_me_goal.is_valid(me) == False:
            self.set_new_target(me, move_me_goal)

    def check_move_valid(self, me):
        move_me_goal = self.subgoals[1]
        # Check that the goal is reachable, and if not skip to a new goal
        if move_me_goal.is_valid(me) == False:
            self.set_new_target(me, move_me_goal)

    def set_new_target(self, me, move_me_goal):
        # print("setting new target")
        waypointName = self.list[randint(0, self.count - 1)]
        waypoint = me.get_knowledge("location", waypointName)

        if not waypoint:
            print("Could not location with name '%s'." % waypointName)
            return

        loc = me.entity.location.copy()
        loc.pos = Point3D([c + uniform(-self.radius, self.radius) for c in waypoint.pos])
        move_me_goal.location = loc
