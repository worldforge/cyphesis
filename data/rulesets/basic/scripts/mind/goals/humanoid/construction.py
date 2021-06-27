# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2004 Al Riddoch (See the file COPYING for details).

import ai
import entity_filter
from atlas import Operation, Entity
from mind.Goal import Goal
from mind.goals.common.misc_goal import SpotSomething, false, AcquireThing, SpotSomethingInArea, ClearFocus
from mind.goals.common.move import PickUpFocus, MoveMeArea, MoveMeToFocus, MoveMeNearFocus


# Gather a resource from nearby
# This is designed to be placed early in a complex goal, so it returns
# as fulfilled when it has nothing to do
class Gather(Goal):
    """Base class for getting a freely available resource."""

    def __init__(self, what, max_amount=1, distance=30):
        Goal.__init__(self, "gather a thing",
                      self.is_there_none_around,
                      [SpotSomething(what),
                       PickUpFocus(what)])
        if isinstance(what, str):
            self.what = what
        else:
            self.what = str(what)
        # FIXME: This goal shares the same filter as spot_something
        self.filter = entity_filter.Filter(self.what)
        self.max_amount = max_amount
        self.distance = distance
        self.vars = ["what", "max_amount", "distance"]

    def is_there_none_around(self, me):
        # Check if we have enough on ourselves
        amount = 0
        entities_in_inventory = me.match_entities(self.filter, me.entity.contains)
        for entity in entities_in_inventory:
            amount += entity.get_prop_int("amount", 1)

        if amount >= self.max_amount:
            return True

        # A suitably range
        what_all = me.map.find_by_filter(self.filter)
        for thing in what_all:
            distance_to_thing = me.steering.distance_to(thing, ai.EDGE, ai.EDGE)
            if distance_to_thing < self.distance:
                return False
        return True


# Harvest a resource from source at a place using a tool
class HarvestResource(Goal):
    """Gather something from a given location, by using a tool on something."""

    def __init__(self, what, source, place, tool, range=30):
        Goal.__init__(self, "Gather a resource using a tool",
                      false,
                      [AcquireThing(tool),
                       MoveMeArea(place, range=range),
                       Gather(what),
                       SpotSomethingInArea(source, location=place, range=range,
                                           condition=self.source_entity_condition, ),
                       MoveMeToFocus(source),
                       self.do])
        self.what = what
        self.source = source
        self.place = place
        self.tool = tool
        self.range = range
        self.vars = ["what", "source", "place", "tool", "range"]

    def source_entity_condition(self, entity):
        if hasattr(entity, "sizeAdult"):
            # Only chop down adult plants which have grown at least 1.1 times their adult size
            # (so that we give the trees some time to disperse seeds, which they only do when they are adult)
            size_adult = entity.sizeAdult
            entity_height = entity.location.bbox.high_corner.y
            if entity_height >= (size_adult * 1.1):
                return True
            return False
        return True

    def do(self, me):
        if (self.tool in me.things) == 0:
            # print "No tool"
            return
        tool = me.find_thing(self.tool)[0]
        if not hasattr(me, 'right_hand_wield') or me.right_hand_wield != tool.id:
            # FIXME We need to sort out how to tell what one is wielding
            return Operation("wield", Entity(tool.id))
        target = me.get_knowledge('focus', self.source)
        if target == None:
            # print "No resource source " + self.source
            return
        return Operation("use", Entity(target, objtype="obj"))


class PlantSeeds(Goal):
    """Use a tool to plant a given kind of seed in a given location."""

    # Get a tool, move to area, look for seed, if found act on seed, if not look for source, move near source. If neither seed nor source is found, roam.
    def __init__(self, seed, source, place, tool, range=30, spacing=4):
        Goal.__init__(self, "Plant seed to grow plants",
                      false,
                      [AcquireThing(tool),
                       MoveMeArea(place, range=range),
                       SpotSomethingInArea(seed, place, range=range, seconds_until_forgotten=360),
                       MoveMeToFocus(seed),
                       self.do,
                       SpotSomethingInArea(source, place, range=range),
                       MoveMeNearFocus(source, allowed_movement_radius=5),
                       ClearFocus(source)])
        self.seed = seed
        self.source = source
        self.source_filter = entity_filter.Filter(source)
        self.place = place
        self.tool = tool
        self.range = range
        self.spacing = spacing
        self.vars = ["seed", "source", "place", "tool", "range", "spacing"]

    def do(self, me):
        if (self.tool in me.things) == 0:
            # print "No tool"
            return
        tool = me.find_thing(self.tool)[0]
        if not hasattr(me, 'right_hand_wield') or me.right_hand_wield != tool.id:
            # FIXME We need to sort out how to tell seed one is wielding
            return Operation("wield", Entity(tool.id))
        id = me.get_knowledge('focus', self.seed)
        if id is None:
            return
        seed = me.map.get(id)
        if seed is None:
            me.remove_knowledge('focus', self.seed)
            return
        if not seed.visible:
            me.remove_knowledge('focus', self.seed)
            return

        # Check that the seed isn't too close to other sources (to prevent us from planting too closely)
        sources_all = me.map.find_by_filter(self.source_filter)
        for thing in sources_all:
            #TODO: add "distance_between"; this won't work
            dist = me.steering.distance_to(thing, ai.EDGE, ai.EDGE)
            if dist is not None and dist < self.spacing:
                # We've found a source which is too close to the seed, so we'll not plant this one
                me.remove_knowledge('focus', self.seed)
                return

        return Operation("use", Entity(seed.id, objtype="obj"))
