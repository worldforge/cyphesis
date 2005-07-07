#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from common import const
from physics import *

if const.server_python:
    from world.physics.Vector3D import Vector3D
else:
    from Vector3D import Vector3D
    from Point3D import Point3D

from mind.goals.common.common import *
try:
  from random import *
except ImportError:
  from whrandom import *

############################ MOVE ME ####################################

class move_me(Goal):
    def __init__(self, location, speed=1):
        Goal.__init__(self,"move me to certain place",
                      self.am_I_at_loc,
                      [self.move_to_loc])
        self.location=location
        self.speed=speed
        self.vars=["location", "speed"]
    def get_location_instance(self, me):
        location_=self.location
        if type(location_)==LambdaType:
            #print "Lambda location"
            location_=location_(me)
        if type(location_)==StringType:
            #print "String location"
            location_=me.get_knowledge("location",location_)
        if not location_:
            #print "Unknown location"
            return None
        return location_
    def am_I_at_loc(self, me):
        location=self.get_location_instance(me)
        if not location:
            #print "No location"
            return 1
        if square_horizontal_distance(me.location, location) < 4: # 1.5 * 1.5
            #print "We are there"
            return 1
        else:
            #print "We are not there"
            return 0
    def move_to_loc(self, me):
        #print "Moving to location"
        location=self.get_location_instance(me)
        if not location:
            #print "but can't - not location"
            return
        # FIXME Destination based movement - currently won't work if
        # a LOC change is required.
        velocity=distance_to(me.location, location).unit_vector()*self.speed
        if abs(velocity.z) > 0.99:
            return
        target=location.copy()
        target.velocity=velocity
        if me.location.velocity.is_valid() and me.location.velocity.dot(target.velocity) > 0.8:
            #print "Already on the way"
            return
        return Operation("move", Entity(me.id, location=target))

############################ MOVE ME AREA ####################################

class move_me_area(Goal):
    def __init__(self, location, range=30):
        Goal.__init__(self, "move me to certain area",
                      self.am_I_in_area,
                      [move_me(location),self.latch_loc])
        self.location=location
        self.range=range
        self.square_range=range*range
        self.arrived=0
        self.vars=["location","range","arrived"]
    def get_location_instance(self, me):
        # FIXME Duplicate of method from move_me() goal
        location_=self.location
        if type(location_)==LambdaType:
            #print "Lambda location"
            location_=location_(me)
        if type(location_)==StringType:
            #print "String location"
            location_=me.get_knowledge("location",location_)
        if not location_:
            #print "Unknown location"
            return None
        return location_
    def am_I_in_area(self, me):
        location=self.get_location_instance(me)
        if not location:
            #print "No location"
            return 0
        if self.arrived:
            #print "Already arrived at location"
            square_dist=square_distance(me.location, location)
            if square_dist > self.square_range:
                self.arrived=0
                #print "Moved away"
                return 0
            else:
                #print "Still here", square_dist, self.square_range
                return 1
        #print "I am not there"
        return 0
    def latch_loc(self, me):
        #print "Latching at location"
        self.arrived=1

############################ MOVE THING ####################################

class move_it(Goal):
    def __init__(self, what, location, speed=0):
        Goal.__init__(self,"move this to certain place",
                      self.is_it_at_loc,
                      [self.move_it_to_loc])
        self.what=what
        self.speed=speed
        self.location=location
        self.wait=0
        self.vars=["what","location","speed","wait"]
    def is_it_at_loc(self, me):
        #CHEAT!: cludge
        if self.wait>0:
            return 0
        if type(self.location)==StringType: 
            self.location=me.get_knowledge("location",self.location)
        if not isLocation(self.location):
            self.location=Location(self.location,Point3D(0.0,0.0,0.0))
        if type(self.what)==StringType:
            if me.things.has_key(self.what)==0: return 1
            what=me.things[self.what][0]
        if what.location.parent.id!=self.location.parent.id: return 0
        return what.location.coordinates.distance(self.location.coordinates)<1.5
    def move_it_to_loc(self, me):
        if self.wait>0:
            self.wait=self.wait-1
            return
        if type(self.location)==StringType: 
            self.location=me.get_knowledge("location",self.location)
        elif not isLocation(self.location):
            self.location=Location(self.location,Point3D(0.0,0.0,0.0))
        if type(self.what)==StringType:
            if me.things.has_key(self.what)==0:
                return
            what=me.things[self.what][0]
        if self.speed==0 or what.location.parent.id!=self.location.parent.id:
            return Operation("move", Entity(what.id, location=self.location))
        iloc=what.location.copy()
        vel=what.location.coordinates.unit_vector_to_another_vector(self.location.coordinates)
        iloc.velocity = vel * self.speed
        self.location.velocity=Vector3D(0.0,0.0,0.0)
        mOp1=Operation("move", Entity(what.id, location=iloc))
        mOp2=Operation("move", Entity(what.id, location=self.location))
        time=((self.location.coordinates-what.location.coordinates).mag() / self.speed)
        self.wait=(time/const.basic_tick)+1
        mOp2.setFutureSeconds(time)
        return Message(mOp1,mOp2)

############################ MOVE THING FROM ME ####################################

class move_it_outof_me(Goal):
    def __init__(self, what):
        Goal.__init__(self, "move this thing from my inventory and disown",
                      self.is_it_not_with_me,
                      [self.drop_it])
        self.what=what
    def is_it_not_with_me(self, me):
        if me.things.has_key(self.what)==0: return 0
        what=me.things[self.what][0]
        return what.location.parent.id!=me.id
    def drop_it(self, me):
        if me.things.has_key(self.what)==0: return
        what=me.things[self.what][0]
        me.remove_thing(what)
        return Operation("move", Entity(what.id, location=me.location))
        
############################ MOVE ME TO THING ##################################

class move_me_to_possession(Goal):
    def __init__(self, what):
        Goal.__init__(self,"move me to this thing",
                      self.am_i_at_it,
                      [self.move_me_to_it])
        self.what=what
        self.vars=["what"]
    def am_i_at_it(self, me):
        what = self.what
        if type(what)==StringType:
            if me.things.has_key(what)==0: return 0
            what=me.things[what][0]
        if square_horizontal_distance(me.location, what.location) < 4: # 2 * 2
            return 1
        else:
            return 0
        
    def move_me_to_it(self, me):
        what = self.what
        if type(what)==StringType:
            if me.things.has_key(what)==0: return
            what=me.things[what][0]
        target=what.location.copy()
        if target.parent.id==me.location.parent.id:
            target.velocity=me.location.coordinates.unit_vector_to_another_vector(target.coordinates)
            target.rotation=target.velocity
            return Operation("move", Entity(me.id, location=target))

class move_me_to_focus(Goal):
    def __init__(self, what):
        Goal.__init__(self,"move me to this thing",
                      self.am_i_at_it,
                      [self.move_me_to_it])
        self.what=what
        self.vars=["what"]
    def am_i_at_it(self, me):
        what = self.what
        if type(what)==StringType:
            id=me.get_knowledge('focus',what)
            if id==None: return 0
            what=me.map.get(id)
            if what==None: return 0
        if square_horizontal_distance(me.location, what.location) < 4: # 2 * 2
            return 1
        else:
            return 0

    def move_me_to_it(self, me):
        what = self.what
        if type(what)==StringType:
            id=me.get_knowledge('focus',what)
            if id==None: return
            what=me.map.get(id)
            if what==None: return
        target=what.location.copy()
        if target.parent.id==me.location.parent.id:
            target.velocity=me.location.coordinates.unit_vector_to_another_vector(target.coordinates)
            target.rotation=target.velocity
            return Operation("move", Entity(me.id, location=target))

############################ MOVE THING TO ME ####################################

class pick_up_possession(Goal):
    def __init__(self, what):
        Goal.__init__(self,"move this thing to my inventory (class)",
                      self.is_it_with_me,
                      [move_me_to_possession(what),
                       self.pick_it_up])
        self.what=what
        self.vars=["what"]
    def is_it_with_me(self, me):
        #CHEAT!: cludge
        what=self.what
        if type(what)==StringType:
            if me.things.has_key(self.what)==0: return 0
            what=me.things[self.what][0]
        if what.location.parent.id!=me.id:
            if what.location.parent.id!=me.location.parent.id:
                me.remove_thing(what.id)
                me.map.delete(what.id)
        return what.location.parent.id==me.id
    def pick_it_up(self, me):
        what=self.what
        if type(what)==StringType:
            if me.things.has_key(self.what)==0: return 0
            what=me.things[self.what][0]
        return Operation("move", Entity(id, location=Location(me, Point3D(0,0,0))))

class pick_up_focus(Goal):
    def __init__(self, what):
        Goal.__init__(self,"move this thing to my inventory (class)",
                      self.is_it_with_me,
                      [move_me_to_focus(what),
                       self.pick_it_up])
        self.what=what
        self.vars=["what"]
    def is_it_with_me(self, me):
        #CHEAT!: cludge
        what=self.what
        if type(what)==StringType:
            id=me.get_knowledge('focus',what)
            if id==None: return 0
            what=me.map.get(id)
            if what==None: return 0
        if what.location.parent.id!=me.id:
            if what.location.parent.id!=me.location.parent.id:
                me.remove_knowledge('focus',self.what)
        return what.location.parent.id==me.id
    def pick_it_up(self, me):
        what=self.what
        if type(what)==StringType:
            id=me.get_knowledge('focus',what)
            me.remove_knowledge('focus',what)
            if id==None: return
            what=me.map.get(id)
            if what==None: return
        return Operation("move", Entity(id, location=Location(me, Point3D(0,0,0))))

############################ WANDER ####################################

class wander(Goal):
    def __init__(self):
        Goal.__init__(self,"wander randomly",false,[self.do_wandering])
    def do_wandering(self, me):
        #copied from build_home.find_place, but changed max amount to -5,5
        loc = me.location.copy()
        loc.coordinates=Point3D(map(lambda c:c+uniform(-5,5),
                                     loc.coordinates))
        ent=Entity(me,location=loc)
        return Operation("move",ent)


############################ WANDER & SEARCH ############################

class search(Goal):
    def __init__(self, me, what):
        Goal.__init__(self, "search for a thing",
                      self.do_I_have,
                      [wander(false),
                       spot_something(what, 30)])
        # Long range for testing only
        self.what=what
        self.vars=["what"]
    def do_I_have(self, me):
        return me.things.has_key(self.what)==1

############################ PURSUIT ####################################

class pursuit(Goal):
    """avoid or hunt something at range"""
    def __init__(self, desc, what, range, direction):
        Goal.__init__(self,"avoid something",self.not_visible,[self.run])
        self.what = what
        self.range = range
        self.direction = direction
        self.vars=["what","range","direction"]
    def not_visible(self, me):
        #print self.__class__.__name__,me.mem.recall_place(me.location,self.range,self.what)
        return not me.mem.recall_place(me.location,self.range,self.what)
    def run(self, me):
        lst_of_what = me.mem.recall_place(me.location,self.range,self.what)
        if lst_of_what==[]: return
        dist_vect=distance_to(me.location,lst_of_what[0].location).unit_vector()
        multiply = const.base_velocity * self.direction
        loc = Location(me.location.parent)
        loc.coordinates =  me.location.coordinates + (dist_vect * multiply)
        #print me,multiply,dist,loc.coordinates.distance(other_xyz)
        ent=Entity(me.id,location=loc)
        return Operation("move",ent)

############################ AVOID ####################################

class avoid(pursuit):
    """avoid something at range"""
    def __init__(self, what, range):
        pursuit.__init__(self,"avoid something",what,range,-1)

################################ HUNT ################################

class hunt(pursuit):
    """hunt something at range"""
    def __init__(self, what, range):
        pursuit.__init__(self,"hunt something",what,range,1)


class hunt_for(pursuit):
    """hunt something at range"""
    def __init__(self, what, range, proximity=5):
        Goal.__init__(self,"hunt for something",
                      self.in_range,
                      [self.run])
        self.what = what
        self.range = range
        self.proximity = proximity
        self.square_proximity = proximity*proximity
        self.direction = 1
        self.vars=["what","range","direction"]
    def in_range(self,me):
        id=me.get_knowledge('focus', self.what)
        if id==None: return
        thing=me.map.get(id)
        if thing==None: return
        square_dist = square_distance(me.location, thing.location)
        return square_dist < self.square_proximity

################################ HUNT ################################

class patrol(Goal):
    def __init__(self, whlist):
        Goal.__init__(self, "patrol an area",
                      false,
                      [move_me(whlist[0]),
                       self.increment])
        self.list = whlist
        self.stage = 0
        self.count = len(whlist)
        self.vars = ["stage", "list"]
    def increment(self, me):
        self.stage = self.stage + 1
        if self.stage >= self.count:
            self.stage = 0
        self.subgoals[0].location = self.list[self.stage]

############################## ACCOMPANY ##############################

class accompany(Goal):
    def __init__(self, who):
        Goal.__init__(self, "stay with someone",
                      self.am_i_with, 
                      [self.follow])
        self.who=who
        self.vars=["who"]
    def am_i_with(self, me):
        who=me.map.get(self.who)
        dist=distance_to(me.location, who.location)
        # Are we further than 3 metres away
        if dist.square_mag() > 25:
            #print "We are far away", dist
            if me.location.velocity.is_valid() and me.location.velocity.dot(dist) > 0.5:
                #print "We moving towards them already"
                return 1
            return 0
        else:
            #print "We are close", dist
            if me.location.velocity.is_valid() and me.location.velocity.dot(dist) < 0.5:
                #print "We going away from them"
                return 0
            return 1
    def follow(self, me):
        who=me.map.get(self.who)
        dist=distance_to(me.location, who.location)
        target = Location(me.location.parent)
        square_dist=dist.square_mag()
        if square_dist > 64:
            #print "We must be far far away - run"
            target.velocity = dist.unit_vector() * 3
        elif square_dist > 25:
            #print "We must be far away - walk"
            target.velocity = dist.unit_vector()
        else:
            #print "We must be close - stop"
            target.velocity = Vector3D(0,0,0)
        return Operation("move", Entity(me.id, location=target))
