# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2004 Al Riddoch (See the file COPYING for details).

import entity_filter
from atlas import Operation, Entity
from physics import square_distance



# Gather a resource from nearby
# This is designed to be placed early in a complex goal, so it returns
# as fulfilled when it has nothing to do
from mind.Goal import Goal
from mind.goals.common.misc_goal import SpotSomething, false, AcquireThing, SpotSomethingInArea, ClearFocus
from mind.goals.common.move import PickUpFocus, MoveMeArea, MoveMeToFocus, MoveMeNearFocus


class Gather(Goal):
    """Base class for getting a freely available resource."""

    def __init__(self, what):
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
        self.vars = ["what"]

    def is_there_none_around(self, me):
        # A suitably range
        square_near_dist = 30
        nearest = None
        nothing_near = 1
        what_all = me.map.find_by_filter(self.filter)
        for thing in what_all:
            square_dist = square_distance(me.entity.location, thing.location)
            if square_dist and square_dist < square_near_dist and thing.location.parent.id != me.entity.id:
                return 0
        return 1


# Harvest a resource from source at a place using a tool
class Harvest_resource(Goal):
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
        spacing_sqr = self.spacing * self.spacing
        for thing in sources_all:
            sqr_dist = square_distance(seed.location, thing.location)
            if sqr_dist and sqr_dist < spacing_sqr:
                # We've found a source which is too close to the seed, so we'll not plant this one
                me.remove_knowledge('focus', self.seed)
                return

        return Operation("use", Entity(seed.id, objtype="obj"))
