#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2004 Al Riddoch (See the file COPYING for details).

from physics import *
from mind.goals.common.misc_goal import *
from mind.goals.common.move import *

# Gather a resource from nearby
# This is designed to be placed early in a complex goal, so it returns
# as fulfilled when it has nothing to do
class gather(Goal):
    def __init__(self, me, what):
        Goal.__init__(self, "gather a thing",
                      self.is_there_none_around,
                      [spot_something(what),
                       move_it_into_me(me, what)])
        self.what=what
        self.vars=["what"]
    def is_there_none_around(self, me):
        # A suitably range
        square_nearest_dist=30
        nearest=None
        what_all=me.map.find_by_type(self.what)
        for thing in what_all:
            square_dist=square_distance(me.location, thing.location)
            if square_dist<square_nearest_dist and thing.location.parent!=me:
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
                       gather(me, what),
                       spot_something(source),
                       move_me_to_it(source),
                       self.do])
        self.wield=False
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
        if not self.wield:
            self.wield=True
            # FIXME We need to sort out how to tell what one is wielding
            return Operation("wield", Entity(tool.id))
        if me.things.has_key(self.source)==0:
            #print "No resource source " + self.source
            return
        target=me.find_thing(self.source)[0]
        return Operation("use",Entity(target.id))
