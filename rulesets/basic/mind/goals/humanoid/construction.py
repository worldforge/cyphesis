#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2004 Al Riddoch (See the file COPYING for details).

from physics import *
from mind.goals.common.misc_goal import *
from mind.goals.common.move import *

# Gather a resource from nearby
# This is designed to be placed early in a complex goal, so it returns
# as fulfilled when it has nothing to do
class gather(Goal):
    def __init__(self, what):
        Goal.__init__(self, "gather a thing",
                      self.is_there_none_around,
                      [spot_something(what),
                       pick_up_focus(what)])
        self.what=what
        self.vars=["what"]
    def is_there_none_around(self, me):
        # A suitably range
        square_nearest_dist=30
        nearest=None
        what_all=me.map.find_by_type(self.what)
        for thing in what_all:
            square_dist=square_distance(me.location, thing.location)
            if square_dist<square_nearest_dist and thing.location.parent.id!=me.id:
                nearest=thing
                square_nearest_dist=square_dist
        if nearest:
            return 0
        else:
            return 1

# Harvest a resource from source at a place using a tool
class harvest_resource(Goal):
    def __init__(self, me, what, source, place, tool):
        Goal.__init__(self, "Gather a resource using a tool",
                      false,
                      [acquire_thing(me,tool),
                       move_me_area(place),
                       gather(what),
                       spot_something(source),
                       move_me_to_focus(source),
                       self.do])
        self.what=what
        self.source=source
        self.place=place
        self.tool=tool
        self.vars=["what","source","place","tool"]
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
    def __init__(self, me, what, source, place, tool):
        Goal.__init__(self, "Plant seed to grow plants",
                      false,
                      [acquire_thing(me, tool),
                       move_me_area(place),
                       spot_something(source),
                       move_me_to_focus(source),
                       spot_something(what),
                       self.do])
        self.what=what
        self.source=source
        self.place=place
        self.tool=tool
        self.vars=["what","source","place","tool"]
    def do(self, me):
        if me.things.has_key(self.tool)==0:
            #print "No tool"
            return
        tool=me.find_thing(self.tool)[0]
        if not hasattr(me, 'right_hand_wield') or me.right_hand_wield!=tool.id:
            # FIXME We need to sort out how to tell what one is wielding
            return Operation("wield", Entity(tool.id))
        id=me.get_knowledge('focus',self.what)
        if id==None:
            return
        what=me.map.get(id)
        if what==None:
            return
        id=me.get_knowledge('focus',self.source)
        if id!=None or me.map.get(id):
            source=me.map.get(id)
            if source!=None:
                if distance_to(source.location, what.location) > 4:
                    return Operation("use",Entity(what.id, objtype="obj"))
        else:
            return Operation("use",Entity(what.id, objtype="obj"))
