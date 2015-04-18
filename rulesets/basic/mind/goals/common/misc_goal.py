#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
#Al Riddoch - Added transport_something() goal

from physics import *
from mind.Goal import Goal
from mind.goals.common.common import *
from mind.goals.common.move import *
from random import *
from entity_filter import *

import time
import types

######################## MAKE LOTS OF SOMETHING ###############################

class make_amount(Goal):
    """Make a certain number of things."""
    def __init__(self, what, amount, what_desc="some thing", place=None):
        Goal.__init__(self,"make certain amount of things",
                      self.are_all_done,
                      [move_me(place),self.do_all])
        self.what=what
        self.amount=amount
        self.what_desc=what_desc
        self.vars=["what","amount"]
    def are_all_done(self, me):
        t_list=me.find_thing(self.what)
        if len(t_list)<self.amount: return 0
        for t in t_list:
            if t.status<1.0-const.fzero: return 0
        return 1
    def do_all(self, me):
        for t in me.find_thing(self.what):
            if t.status<1.0-const.fzero: 
                return Operation("set",Entity(t.id,status=t.status+0.1))
        return Operation("create",Entity(name=self.what,parents=[self.what],
                                         description=self.what_desc))

########################## NOT YET IMPLEMENTED ################################

class nyi(Goal):
    """Perform a task that is not yet implemented."""
    def __init__(self, desc):
        Goal.__init__(self,desc,
                      lambda self:0,
                      [self.not_yet_implemented])
        self.vars=["desc"]
    def not_yet_implemented(self, me):
        #CHEAT! (make it so that it asks other people for knowledge)
        return

############################ IMAGINARY TASK ###################################

class imaginary(Goal):
    """Perform a task that should be broadcast without really doing it."""
    def __init__(self, desc, time, place):
        Goal.__init__(self,desc,false,[move_me(place),self.imaginary],time)
        self.vars=["desc"]
    def imaginary(self,me):
        return Operation("imaginary",Entity(description=self.desc))

####################### GET KNOWLEDEGE ABOUT THING ############################

class get_knowledge(Goal):
    """Find out what we remember about a thing."""
    def __init__(self, what):
        Goal.__init__(self,"get knowledge about thing by name",
                      self.do_I_know_about_this,
                      [nyi("Try another task or try asking around!")])
        self.what=what
        self.vars=["what"]
    def do_I_know_about_this(self, me):
        if me.things.has_key(self.what):
            return 1
        if me.get_knowledge("place",self.what):
            return 1
        return 0
            

############################ BUY KNOWN THING ##################################

class get_thing(Goal):
    """Base class for obtaining something."""
    def do_I_have_it(self, me):
        return me.things.has_key(self.what)

class buy_thing(get_thing):
    """Purchase something from a location."""
    def __init__(self, what):
        #CHEAT!: doesn't work for static goals ;-(
        #goal systems needs redesign
        Goal.__init__(self,"buy thing by name (I am at shop)",
                      self.do_I_have_it,
                      [move_me_place(what),
                       self.buy_it])
        self.what=what
        self.vars=["what"]
    def buy_it(self, me):
        return Operation("talk",Entity(say="I would like to buy an "+self.what))
#        e=event("say",what=esay("I want to buy "+self.what,
#                                verb='buy',subject=self.what))
#        #check if I already did this
#        if me.mem.recall_event(e,lambda e1,e2:e1.command==e2.command\
#                               and e1.what==e2.what):
#            return []
#        #no I didn't: do it
#        return [e]

############################ ACQUIRE THING: BASE ##############################

class acquire(Goal):
    """Base class for getting something into inventory."""
    def is_it_in_my_inventory(self, me):
        if me.things.has_key(self.what):
            return me.things[self.what][0].location.parent==me
        return 0

############################ ACQUIRE KNOWN THING ##############################

class acquire_known_thing(acquire):
    """Buy something into inventory."""
    def __init__(self, what):
        Goal.__init__(self,"acquire known thing by name",
                      self.is_it_in_my_inventory,
                      [buy_thing(what),
                       pick_up_possession(what)])
        self.what=what
        self.vars=["what"]

############################ ACQUIRE THING ####################################

class acquire_thing(acquire):
    """Buy semething from a known place into inventory."""
    def __init__(self, what):
        Goal.__init__(self,"acquire thing by name",
                      self.is_it_in_my_inventory,
                      [get_knowledge(what),
                       acquire_known_thing(what)])
        self.what=what
        self.vars=["what"]
        
############################ TASK WITH PLACE AND TOOL #########################

class task(Goal):
    """Base class for performing a task with a tool."""
    def __init__(self, desc, time, place, what, tool):
        Goal.__init__(self,desc,
                      false,
                      [acquire_thing(tool),
                       move_me_area(place),
                       spot_something(what),
                       move_me_to_focus(what),
                       self.do],
                      time)
        self.tool=tool
        self.what=what
        self.vars=["tool","what"]
    def do(self,me):
        return Operation("imaginary",Entity(description=self.desc))

############################ CUT TREES TASK #############################

class cut_something(task):
    """Use a tool to cut down a tree."""
    def do(self,me):
        if me.things.has_key(self.what)==0: return
        if me.things.has_key(self.tool)==0: return
        if not hasattr(self,'wield') or not self.wield:
            tool=me.find_thing(self.tool)[0]
            self.wield=True
            return Operation("wield", Entity(tool.id))
        what=me.find_thing(self.what)[0]
        return Operation("use",Entity(what.id))

############################ SPOT SOMETHING GOAL ########################

class spot_something(Goal):
    """Pick out something and focus on it, forgetting things previously focused on after a while."""
    def __init__(self, what, range=30, condition=lambda a:1, seconds_until_forgotten=30):
        Goal.__init__(self, "spot a thing",
                      self.do_I_have,
                      [self.do])
        if isinstance(what, str):
            self.what = what
        else:
            self.what = str(what)

        self.filter = get_filter(what)

        self.range=range
        self.condition=condition
        #Keep track of when we've spotted things, so that we can ignore them
        #seconds_until_forgotten is used to forget about spotted things. That way we can re-discover things 
        #we've previously focused on after a while.
        self.spotted={}
        #How many seconds until we've forgotten something we've previously focused on.
        self.seconds_until_forgotten=seconds_until_forgotten
        self.vars=["what","range", "seconds_until_forgotten"]
    def do_I_have(self, me):
        something=me.get_knowledge('focus', self.what)
        if something:
            if me.map.get(something) == None:
               me.remove_knowledge('focus', self.what)
            else:
                #Update the time since we last knew about the thing, as we're still actively know about it
                if self.seconds_until_forgotten > 0:
                    self.spotted[something] = time.time()
                return 1
    def do(self,me):
        thing_all=me.map.find_by_filter(self.filter)
        nearest=None
        nearsqrdist=self.range*self.range
        for thing in thing_all:
            #Check that it's not something we've already spotted
            if thing.id in self.spotted:
                #Have we forgotten about that we remembered it yet?
                if time.time() - self.spotted[thing.id] > self.seconds_until_forgotten:
                    self.spotted.pop(thing.id)
                else:
                    continue

            sqr_dist = square_distance(me.location, thing.location)
            # FIXME We need a more sophisticated check for parent. Perhaps just
            # check its not in a persons inventory? Requires the ability to
            # do decent type checks
            if sqr_dist < nearsqrdist and thing.location.parent.id==me.location.parent.id:
                if self.condition(thing):
                    nearest = thing
                    nearsqrdist = nearsqrdist
        if nearest:
            me.add_knowledge('focus', self.what, nearest.id)
            #We should only remember things if we can keep them in memory.
            if self.seconds_until_forgotten > 0:
                self.spotted[nearest.id] = time.time()
                      
class spot_something_in_area(Goal):
    """Pick out something in a specified area and focus on it, forgetting things previously focused on after a while."""
    def __init__(self, what, location, range=30, condition=lambda a:1, seconds_until_forgotten=30):
        Goal.__init__(self, "spot a thing in area",
                      self.do_I_have,
                      [self.do])
        self.range = range
        self.sqr_range = range*range
        self.area_location_name = location
        self.condition=condition
        self.inner_spot_goal = spot_something(what, range=range, seconds_until_forgotten=seconds_until_forgotten, condition=self.is_thing_in_area)
    def do_I_have(self, me):
        self.inner_spot_goal.do_I_have(me)
    def do(self,me):
        #For performance reasons we'll cache the current location, so we don't have to look it up for each call to is_thing_in_area
        self.current_location = me.get_knowledge("location",self.area_location_name)
        self.inner_spot_goal.do(me)
    def is_thing_in_area(self, thing):
        if self.condition(thing):
            if self.current_location is None:
                return False
            sqr_dist = square_distance(self.current_location, thing.location)
            if sqr_dist < self.sqr_range:
                return True
        return False
         

############################ FETCH SOMETHING GOAL ########################

class fetch_something(Goal):
    """Get something that is freely available from a known location."""
    def __init__(self, what, where):
        Goal.__init__(self, "fetch a thing",
                      self.is_it_in_my_inventory,
                      [move_me_area(where, 20),
                       spot_something(what),
                       pick_up_focus(what)])
        self.what=what
        self.vars=["what"]
    def is_it_in_my_inventory(self,me):
        if me.things.has_key(self.what):
            if me.things[self.what][0].location.parent==me:
                me.remove_knowledge('focus', self.what)
                return 1
        return 0

############################ MOVE SOMETHING GOAL ########################

class transport_something(Goal):
    """Move things of a given type from one location to another."""
    def __init__(self, what, src, dest):
        Goal.__init__(self,"move thing to place",
                      false,
                      [fetch_something(what, src),
                       move_me(dest),
                       move_it_outof_me(what)])
        self.what=what
        self.vars=["what"]

############################### SIT DOWN ######################################

class sit_down(Goal):
    """Sit down and rest in a given location."""
    def __init__(self, where):
        Goal.__init__(self,"sit down",
                      self.am_i_sat,
                      [spot_something(where),
                       move_me_to_focus(where),
                       sit])
    def am_i_sat(self,me):
        return me.mode=="sitting"
    def sit(self,me):
        return Operation("set", Entity(me.id, mode="sitting"))

############################ MOVE AROUND RANDOMLY ###########################################

class amble(Goal):
    """Move randomly if not already moving."""
    def __init__(self) :
        Goal.__init__(self, "move in a random direction if not already in motion",
                      self.am_i_moving,
                      [self.do_amble])
        self.what="world"
    
    def am_i_moving(self,me):
        if me.location.velocity:
            return 0
        return 1

    def do_amble(self, me):
        id=me.get_knowledge('focus', 'hook')
        if id != None: 
            thing = me.map.get(id)
            if thing == None:
                me.remove_knowledge('focus', what)
            else:
                if thing.location.parent.id != me.location.parent.id:
                    me.remove_knowledge('focus', what)
                else:
                    if thing.location.parent.id == me.id:
                        return
        #world = 
        #ground = world.id 
        #op = Operation("eat", ground)
        #print "Fish ambling"
        target = Location(me.location.parent, me.location.coordinates)
        target.coordinates = Vector3D(target.coordinates.x + uniform(-1.5,1.5), target.coordinates.y+ uniform(-1.5,1.5), target.coordinates.z)
        target.velocity = Vector3D(1,0,0)
        return Operation("move",  Entity(me.id, location=target))

############################ FEED (FOR FOOD) ##################################

class feed(Goal):
    """Eat something."""
    def eat(self,me):
        me.remove_knowledge('focus',self.what)
        if me.things.has_key(self.what)==0: return
        food=me.find_thing(self.what)[0]
        ent=Entity(food.id)
        return Operation("eat",ent)
    def am_i_full(self,me):
        if hasattr(me,"food") and hasattr(me,"mass"):
            if me.food > (self.full * me.mass):
                return 1
            if me.food < (self.full * me.mass / 4):
                self.subgoals[0].range = self.range * 2
            else:
                self.subgoals[0].range = self.range
        return 0

########################## BUY AND EAT A MEAL #################################

class meal(feed):
    """Buy a meal from a given location and eat it."""
    def __init__(self, what, time, place, seat=None):
        if seat==None:
            Goal.__init__(self,"have a meal",
                          self.am_i_full,
                          [move_me_area(place,10),
                           acquire_thing(what),
                           self.eat],time)
        else:
            Goal.__init__(self,"have a meal",
                          self.am_i_full,
                          [move_me_area(place,10),
                           acquire_thing(what),
                           sit_down(seat),
                           self.eat],time)
        self.what=what
        self.place=place
        self.full=0.01
        self.vars=["what","place"]
    def imaginary(self,me):
        return Operation("imaginary",Entity(description=self.desc))

############################ FORAGE (FOR FOOD) ################################

class forage(feed):
    """Find food available in a given location and eat it."""
    def __init__(self, what):
        Goal.__init__(self, "forage for food by name",
                      self.am_i_full,
                      [spot_something(what, range=5),
                       pick_up_focus(what),
                       self.eat])
        self.what=what
        self.range=5
        self.full=0.2
        self.vars=["what","range"]

############################ PECK ###########################################

class peck(feed):
    """Peck at food in a given location."""
    def __init__(self) :
        Goal.__init__(self, "forage for food by name and eat randomly",
                      self.am_i_full,
                      [self.do_peck])
        self.what="world"
        self.full=0.2
        self.vars=["full"]

    def do_peck(self, me):
        #world = 
        #ground = world.id 
        #op = Operation("eat", ground)
        target = Location(me.location.parent, me.location.coordinates)
        target.coordinates = Vector3D(target.coordinates.x + uniform(-1.5,1.5), target.coordinates.y+ uniform(-1.5,1.5), target.coordinates.z)
        target.velocity = Vector3D(1,0,0)
        #op += Operation("move",  Entity(me.id, location=target))
        return Operation("move",  Entity(me.id, location=target))
                                      


############################ BROWSE (FIND FOOD, EAT SOME, MOVE ON) ###########

class browse(feed):
    """Browse for food in a given location."""
    def __init__(self, what, min_status):
        Goal.__init__(self, "browse for food by name",
                      self.am_i_full,
                      [spot_something(what, range=20, condition=(lambda o,s=min_status:hasattr(o,"status") and o.status > s)),
                       move_me_to_focus(what),
                       self.eat])
        self.what=what
        self.range=20
        self.full=0.2
        self.min_status=min_status
        self.vars=["what", "range", "min_status"]


    def eat(self,me):
        if me.things.has_key(self.what)==0: return
        food=me.find_thing(self.what)[0]
        if food.status < self.min_status:
            me.remove_thing(food)
        ent=Entity(food.id)
        return Operation("eat",ent)


############################ PREDATOR (HUNT SOMETHING, THEN EAT IT) ###########

class predate(feed):
    """Hunt for a specified type of prey and eat it."""
    def __init__(self, what, range):
        Goal.__init__(self, "predate something",
                      self.am_i_full,
                      [spot_something(what, range=range),
                       hunt_for(what, range),
                       self.eat])
        self.what=what
        self.range=range
        self.full=0.1
        self.vars=["what","range"]

class predate_small(feed):
    """Hunt for a specified type of prey with a small mass and eat it."""
    def __init__(self, what, range, max_mass):
        Goal.__init__(self, "predate something",
                      self.am_i_full,
                      [spot_something(what, range, condition=(lambda o,m=max_mass:hasattr(o,"mass") and o.mass < m)),
                       hunt_for(what, range),
                       self.eat])
        self.what=what
        self.range=range
        self.max_mass=max_mass
        self.full=0.1
        self.vars=["what","range"]

################### HUNT (SEARCH FOR SOMETHING, THEN KILL IT) #################

class hunt(Goal):
    """Hunt for something, then kill it with a weapon."""
    def __init__(self, weapon, what, range):
        Goal.__init__(self, "hunt something",
                      self.none_in_range,
                      [spot_something(what, range),
                       acquire_thing(weapon),
                       hunt_for(what, range, 7),
                       self.fight])
        self.weapon=weapon
        self.what=what
        self.filter=get_filter(what)
        self.range=range
        self.square_range=range*range
        self.vars=["weapon", "what", "range"]
    def none_in_range(self, me):
        thing_all=me.map.find_by_filter(self.filter)
        for thing in thing_all:
            if square_distance(me.location, thing.location) < self.square_range:
                return 0
        return 1
    def fight(self, me):
        if me.things.has_key(self.weapon)==0:
            print "no weapon"
            return
        weapon=me.find_thing(self.weapon)[0]
        if me.things.has_key(weapon.ammo)==0:
            print "no ammo"
            return
        ammo=me.find_thing(weapon.ammo)[0]
        if me.things.has_key(self.what)==0:
            print "no target"
            return
        enemy=me.find_thing(self.what)[0]
        me.remove_thing(ammo)
        return Operation("shoot",Entity(ammo.id),Entity(enemy.id),to=weapon)

##################### DEFEND (SPOT SOMETHING, THEN KILL IT) ###################

class defend(Goal):
    """Kill something if we see it."""
    def __init__(self, what, range):
        Goal.__init__(self, "defend against something",
                      self.none_in_sight,
                      [spot_something(what),
                       hunt_for(what, range),
                       self.fight])
        self.what=what
        self.filter=get_filter(what)
        self.range=range
        self.vars=["what", "range"]
    def none_in_sight(self, me):
        target_id=me.get_knowledge('focus', self.what)
        if target_id:
            return 0
        thing_all=me.map.find_by_filter(self.filter)
        if thing_all and len(thing_all) > 0:
            return 0
        return 1
    def fight(self, me):
        target_id=me.get_knowledge('focus', self.what)
        if not target_id:
            return
        return Operation("attack",Entity(target_id))

############################ BUY SOMETHING (AS TRADER) ########################

class buy_trade(get_thing):
    """Go to a location and offer to buy a certain thing."""
    def __init__(self, what, where, when=None):
        Goal.__init__(self, "buy from the public",
                      self.do_I_have_it,
                      [move_me(where),
                       self.announce_trade],
                      when)
        self.what=what
        self.ticks=0
        self.vars=["what"]
    def announce_trade(self,me):
        self.ticks=self.ticks+1
        if self.ticks==5:
            self.ticks=0
            es=Entity(say="Sell your " + self.what + " here!")
            return Operation("talk",es)

############################ SELL SOMETHING (AS TRADER) ########################

class sell_trade(Goal):
    """Go to a location and off to sell a certain thing."""
    def __init__(self, what, where, when=None):
        Goal.__init__(self, "Sell to the public",
                      self.dont_I_have_it,
                      [move_me(where),
                       self.announce_trade],
                      when)
        self.what=what
        self.ticks=0
        self.vars=["what"]
    def dont_I_have_it(self, me):
        if me.things.has_key(self.what):
            return randint(0,1)
        else:
            return 1
    def announce_trade(self,me):
        self.ticks=self.ticks+1
        ret = Oplist()
        if self.ticks==5:
            self.ticks=0
            es=Entity(say="Get your " + self.what + " here!")
            ret = ret + Operation("talk",es)
            ret = ret + Operation("imaginary", Entity("shout"))
        else:
            if randint(0,4)==1:
                ret = ret + Operation("imaginary", Entity("wave"))
        return ret

################ TRADE (BUY SOMETHING, USE TOOL, SELL PRODUCT) ################

class trade(Goal):
    """Offer to buy something, then use a tool on it, and sell the product."""
    def __init__(self, wbuy, tool, wsell, where, when=None):
        Goal.__init__(self, "trade at a market",
                      false,
                      [acquire_thing(tool),
                       buy_trade(wbuy, where),
                       self.process],
                      when)
        self.wbuy=wbuy
        self.tool=tool
        self.wsell=wsell
        self.wield=False
        self.vars=["wbuy","tool","wsell"]
    def process(self,me):
        if me.things.has_key(self.wbuy)==0: return
        if me.things.has_key(self.tool)==0: return
        tool=me.find_thing(self.tool)[0]
        if not self.wield:
            self.wield=True
            return Operation("wield",Entity(tool.id))
        thing=me.find_thing(self.wbuy)[0]
        return Operation("use",Entity(thing.id, objtype="obj"))

############################# RUN MARKET STALL ##############################

class market(Goal):
    """Base class to run a market stall."""
    def is_it(self,me):
        if me.things.has_key(self.shop)==0: return 0
        shop=me.find_thing(self.shop)[0]
        return shop.mode==self.state
    def set_it(self,me):
        if me.things.has_key(self.shop)==0: return
        shop=me.find_thing(self.shop)[0]
        return Operation("set", Entity(shop.id, mode=self.state))

class run_shop(market):
    """Run a market stall."""
    def __init__(self, shop, updown, time):
        Goal.__init__(self, "run a shop",
                      self.is_it,
                      [spot_something(shop),
                       self.set_it],time)
        # FIXME This probably does not work, but I'll fix it when we need it
        self.shop=shop
        self.state=updown
        self.vars=["shop"]

######################### KEEP (Things that I own in place) #################

class keep(Goal):
    """Make sure everything we own of a given type is kept in a place."""
    def __init__(self, what, where):
        Goal.__init__(self, "Keep "+what+" in "+where+".",
                      self.are_they_there,
                      [self.keep_it])
        self.what=what
        self.where=where
        self.vars=["what","where"]
    def are_they_there(self,me):
        if me.things.has_key(self.where)==0: return 1
        if me.things.has_key(self.what)==0: return 1
        thing_all=me.find_thing(self.what)
        where=me.find_thing(self.where)[0]
        for thing in thing_all:
            if thing.location.parent.id!=where.id and thing.location.parent.id!=me.id:
                return 0
        return 1
    def keep_it(self,me):
        result=Oplist()
        if me.things.has_key(self.where)==0: return
        if me.things.has_key(self.what)==0: return
        thing_all=me.find_thing(self.what)
        where=me.find_thing(self.where)[0]
        to_location=Location(where,Point3D(0,0,0))
        minx=where.location.bbox.near_point.x
        miny=where.location.bbox.near_point.y
        maxx=where.location.bbox.far_point.x
        maxy=where.location.bbox.far_point.y
        for thing in thing_all:
            if thing.location.parent.id!=where.id and thing.location.parent.id!=me.id:
                thingloc=Location(where,Point3D(uniform(minx,maxx),uniform(miny,maxy),0))
                result.append(Operation("move",Entity(thing.id, location=thingloc)))
        return result

################## KEEP ON ME (money kept on my person) ################

class keep_on_me(Goal):
    """Make sure everything we own of a given type is in inventory."""
    def __init__(self,what):
        Goal.__init__(self, "Keep "+what+" on me.",
                      self.are_they_on_me,
                      [self.keep_it])
        self.what=what
        self.vars=["what"]
    def are_they_on_me(self,me):
        thing_all=me.find_thing(self.what)
        for thing in thing_all:
            if thing.location.parent.id!=me.id:
                return 0
        return 1
    def keep_it(self,me):
        result=Oplist()
        if me.things.has_key(self.what)==0: return
        thing_all=me.find_thing(self.what)
        to_loc=Location(me,Point3D(0,0,0))
        for thing in thing_all:
            if thing.location.parent.id!=me.id:
                result.append(Operation("move",Entity(thing.id, location=to_loc)))
        return result

################## ASSEMBLE (Thing from list of parts) #################

class assemble(Goal):
    """Assemble something from a list of parts."""
    def __init__(self, me, what, fromwhat):
        sgoals=[]
        for item in fromwhat:
            sgoals.append(spot_something(item))
            sgoals.append(move_it(item, me.location, 1))
        sgoals.append(self.build)
        Goal.__init__(self, "Build "+what+" from parts.",
                     false, sgoals)
        self.what=what
        self.fromwhat=fromwhat
        self.vars=["what","fromwhat"]
    def build(self, me):
        retops=Oplist()
        for item in self.fromwhat:
            if me.things.has_key(item)==0: return
        for item in self.fromwhat:
            cmpnt=me.find_thing(item)[0]
            retops = retops + Operation("set", Entity(cmpnt.id,status=-1))
        retops = retops + Operation("create", Entity(name=self.what,parents=[self.what], location=me.location.copy()))
        retops = retops + Operation("imaginary", Entity("conjure"))
        return retops

######################## TRANSACTION (Sell thing) #######################

class transaction(Goal):
    """Conduct a transaction."""
    def __init__(self, what, who, cost):
        Goal.__init__(self,"conduct transaction",
                      self.transaction_inactive,
                      [self.transact])
        self.what=what
        self.who=who
        self.cost=int(cost)
        self.payed=0
        self.vars=["what", "who", "cost", "payed"]
    def check_availability(self,me):
        return me.things.has_key(self.what)==0
    def transaction_inactive(self,me):
        if self.check_availability(me):
            return 0
        if len(me.money_transfers)==0:
            return 1
        payed=self.payed
        if payed==-1: payed=0
        for item in me.money_transfers:
            if item[0]==self.who:
                payed=payed+int(item[1])
        for item in me.money_transfers:
            if item[0]==self.who:
                me.money_transfers.remove(item)
        if payed != self.payed:
            self.payed=payed
            return 0
        return 1
    def transact(self,me):
        who=me.map.get(self.who)
        if me.things.has_key(self.what)==0:
            self.irrelevant=1
            return Operation("talk",Entity(say="I don't have any "+self.what+" left."))
        if self.payed < self.cost:
            return Operation("talk",Entity(say=who.name+" you owe me "+str(self.cost-self.payed)+" coins."))
        thing=me.find_thing(self.what)[0]
        res=Oplist()
        me.remove_thing(thing)
        res.append(Operation("move",Entity(thing.id, location=Location(who,Point3D(0,0,0)))))
        res.append(Operation("talk",Entity(say="Thankyou for your custom.")))
        self.irrelevant=1
        return res

class hireling_transaction(transaction):
    """Conduct a transaction to hire our services."""
    def check_availability(self,me):
        return me.get_knowledge('employer', me) != None
    def transact(self, me):
        employer = me.get_knowledge('employer', me)
        if employer:
            print 'Already employed by ' + employer
            return Operation("talk",Entity(say="Sorry, I am currently working for someone else."))
        who=me.map.get(self.who)
        if not who:
            print "Who am I talking to"
            return
        if self.payed < self.cost:
            return Operation("talk",Entity(say=who.name+" you owe me "+str(self.cost-self.payed)+" coins."))
        res=Oplist()
        me.add_knowledge('employer', me.id, who.id)
        # FIXME add the new goal
        goal = accompany(who.id)
        me.goals.insert(0,goal)
        res.append(Operation("talk",Entity(say="I will help you out until sundown today.")))
        self.irrelevant=1
        return res

######################## ACTUATION (Operate device) #######################

class activate_device(Goal):
    """Look for something and activate it."""
    def __init__(self, what):
        Goal.__init__(self, "activate a thing",
                      self.activated,
                      [spot_something(what), self.activate_focus])
        self.what=what
        self.vars=["what"]
    def activated(self, me):
        return False
    def activate_focus(self, me):
        print "Activating ", self.what
        something=me.get_knowledge('focus', self.what)
        assert(something)
        if me.map.get(something) == None:
           me.remove_knowledge('focus', self.what)
           return
        return Operation('actuate', Operation('chop', Entity(something)))
    
    
class clear_focus(Goal):
    """Clear whatever is in the current focus."""
    def __init__(self, what):
        Goal.__init__(self, "clear focus",
                      self.clear_focus,
                      [])
        if isinstance(what, list):
            self.what = what
        else:
            self.what = [ what ]
        self.vars=["what"]
    def clear_focus(self, me):
        for what in self.what:
            something=me.get_knowledge('focus', what)
            if something:
                me.remove_knowledge('focus', what)
        return 1
