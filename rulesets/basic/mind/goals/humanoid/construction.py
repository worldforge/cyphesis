#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2004 Al Riddoch (See the file COPYING for details).

from physics import *
from mind.goals.common.misc_goal import *
from mind.goals.common.move import *

import types

# Gather a resource from nearby
# This is designed to be placed early in a complex goal, so it returns
# as fulfilled when it has nothing to do
class gather(Goal):
    """Base class for getting a freely available resource."""
    def __init__(self, what):
        Goal.__init__(self, "gather a thing",
                      self.is_there_none_around,
                      [spot_something(what),
                       pick_up_focus(what)])
        if type(what) == types.ListType:
            self.what = what
        else:
            self.what = [ what ]
        self.vars=["what"]
    def is_there_none_around(self, me):
        # A suitably range
        square_near_dist=30
        nearest=None
        nothing_near = 1
        for what in self.what:
            what_all=me.map.find_by_type(what)
            for thing in what_all:
                square_dist=square_distance(me.location, thing.location)
                if square_dist < square_near_dist and \
                   thing.location.parent.id != me.id:
                    return 0
        return 1

# Harvest a resource from source at a place using a tool
class harvest_resource(Goal):
    """Gather something from a given location, by using a tool on something."""
    def __init__(self, what, source, place, tool):
        Goal.__init__(self, "Gather a resource using a tool",
                      false,
                      [acquire_thing(tool),
                       move_me_area(place),
                       gather(what),
                       spot_something_in_area(source, location=place, range=30, condition=self.source_entity_condition, ),
                       move_me_to_focus(source),
                       self.do])
        self.what=what
        self.source=source
        self.place=place
        self.tool=tool
        self.vars=["what","source","place","tool"]
        
    def source_entity_condition(self, entity):
        if hasattr(entity, "sizeAdult"):
            #Only chop down adult plants which have grown at least 1.1 times their adult size
            #(so that we give the trees some time to disperse seeds, which they only do when they are adult)
            sizeAdult = entity.sizeAdult
            entity_height = entity.location.bbox.far_point.z 
            if entity_height >= (sizeAdult * 1.1):
                return True
            return False
        return True 
        
    def do(self, me):
        if me.things.has_key(self.tool)==0:
            #print "No tool"
            return
        tool=me.find_thing(self.tool)[0]
        if not hasattr(me, 'right_hand_wield') or me.right_hand_wield!=tool.id:
            # FIXME We need to sort out how to tell what one is wielding
            return Operation("wield", Entity(tool.id))
        target=me.get_knowledge('focus', self.source)
        if target==None:
            #print "No resource source " + self.source
            return
        return Operation("use",Entity(target, objtype="obj"))

class plant_seeds(Goal):
    """Use a tool to plant a given kind of seed in a given location."""
    #Get a tool, move to area, look for seed, if found act on seed, if not look for source, move near source. If neither seed nor source is found, roam.
    def __init__(self, seed, source, place, tool, range=30):
        Goal.__init__(self, "Plant seed to grow plants",
                      false,
                      [acquire_thing(tool),
                       move_me_area(place, range=range),
                       spot_something_in_area(seed, place, range=range),
                       move_me_to_focus(seed),
                       self.do,
                       spot_something_in_area(source, place, range=range),
                       move_me_near_focus(source, allowed_movement_radius=5),
                       clear_focus(source)])
        self.seed=seed
        self.source=source
        self.place=place
        self.tool=tool
        self.range=range
        self.vars=["seed","source","place","tool","range"]
    def do(self, me):
        if me.things.has_key(self.tool)==0:
            #print "No tool"
            return
        tool=me.find_thing(self.tool)[0]
        if not hasattr(me, 'right_hand_wield') or me.right_hand_wield!=tool.id:
            # FIXME We need to sort out how to tell seed one is wielding
            return Operation("wield", Entity(tool.id))
        id=me.get_knowledge('focus',self.seed)
        if id==None:
            return
        seed=me.map.get(id)
        if seed==None:
            return
        if seed.visible == False:
            return
        return Operation("use",Entity(seed.id, objtype="obj"))
