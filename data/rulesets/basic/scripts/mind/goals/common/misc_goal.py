# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).
# Al Riddoch - Added transport_something() goal

from random import *

import entity_filter
import ai
from atlas import Operation, Entity, Oplist
from common import const
from physics import Vector3D, Point3D
from rules import Location

from mind.goals.common.common import *
from mind.goals.common.move import MoveMePlace, MoveMe, PickUpPossession, MoveMeArea, MoveMeToFocus, PickUpFocus, \
    MoveItOutOfMe, HuntFor, MoveIt, Accompany


######################## MAKE LOTS OF SOMETHING ###############################

class MakeAmount(Goal):
    """Make a certain number of things."""

    def __init__(self, what, amount, what_desc="some thing", place=None):
        Goal.__init__(self, "make certain amount of things",
                      self.are_all_done,
                      [MoveMe(place), self.do_all])
        self.what = what
        self.amount = amount
        self.what_desc = what_desc
        self.vars = ["what", "amount"]

    def are_all_done(self, me):
        t_list = me.find_thing(self.what)
        if len(t_list) < self.amount: return 0
        for t in t_list:
            if t.props.status < 1.0 - const.fzero: return 0
        return 1

    def do_all(self, me):
        for t in me.find_thing(self.what):
            if t.props.status < 1.0 - const.fzero:
                return Operation("set", Entity(t.id, status=t.props.status + 0.1))
        return Operation("create", Entity(name=self.what, parent=self.what,
                                          description=self.what_desc))


########################## NOT YET IMPLEMENTED ################################

class NYI(Goal):
    """Perform a task that is not yet implemented."""

    def __init__(self, desc):
        Goal.__init__(self, desc,
                      lambda self: 0,
                      [self.not_yet_implemented])
        self.vars = ["desc"]

    def not_yet_implemented(self, me):
        # CHEAT! (make it so that it asks other people for knowledge)
        return


############################ IMAGINARY TASK ###################################

class Imaginary(Goal):
    """Perform a task that should be broadcast without really doing it."""

    def __init__(self, desc, place):
        Goal.__init__(self, desc, false, [MoveMe(place), self.imaginary])
        self.vars = ["desc"]

    def imaginary(self, me):
        return Operation("imaginary", Entity(description=self.desc))


####################### GET KNOWLEDEGE ABOUT THING ############################

class GetKnowledge(Goal):
    """Find out what we remember about a thing."""

    def __init__(self, what):
        Goal.__init__(self, "get knowledge about thing by name",
                      self.do_I_know_about_this,
                      [NYI("Try another task or try asking around!")])
        self.what = what
        self.vars = ["what"]

    def do_I_know_about_this(self, me):
        if self.what in me.things:
            return 1
        if me.get_knowledge("place", self.what):
            return 1
        return 0


############################ BUY KNOWN THING ##################################

class GetThing(Goal):
    """Base class for obtaining something."""

    def do_I_have_it(self, me):
        return self.what in me.things


class BuyThing(GetThing):
    """Purchase something from a location."""

    def __init__(self, what):
        # CHEAT!: doesn't work for static goals ;-(
        # goal systems needs redesign
        Goal.__init__(self, "buy thing by name (I am at shop)",
                      self.do_I_have_it,
                      [MoveMePlace(what),
                       self.buy_it])
        self.what = what
        self.vars = ["what"]

    def buy_it(self, me):
        return Operation("talk", Entity(say="I would like to buy an " + self.what))


#        e=event("say",what=esay("I want to buy "+self.what,
#                                verb='buy',subject=self.what))
#        #check if I already did this
#        if me.mem.recall_event(e,lambda e1,e2:e1.command==e2.command\
#                               and e1.what==e2.what):
#            return []
#        #no I didn't: do it
#        return [e]

############################ ACQUIRE THING: BASE ##############################

class Acquire(Goal):
    """Base class for getting something into inventory."""

    def is_it_in_my_inventory(self, me):
        if self.what in me.things:
            return me.things[self.what][0].parent == me
        return 0


############################ ACQUIRE KNOWN THING ##############################

class AcquireKnownThing(Acquire):
    """Buy something into inventory."""

    def __init__(self, what):
        Goal.__init__(self, "acquire known thing by name",
                      self.is_it_in_my_inventory,
                      [BuyThing(what),
                       PickUpPossession(what)])
        self.what = what
        self.vars = ["what"]


############################ ACQUIRE THING ####################################

class AcquireThing(Acquire):
    """Buy semething from a known place into inventory."""

    def __init__(self, what):
        Goal.__init__(self, "acquire thing by name",
                      self.is_it_in_my_inventory,
                      [GetKnowledge(what),
                       AcquireKnownThing(what)])
        self.what = what
        self.vars = ["what"]


############################ TASK WITH PLACE AND TOOL #########################

class Task(Goal):
    """Base class for performing a task with a tool."""

    def __init__(self, desc, place, what, tool):
        Goal.__init__(self, desc,
                      false,
                      [AcquireThing(tool),
                       MoveMeArea(place),
                       SpotSomething(what),
                       MoveMeToFocus(what),
                       self.do]
                      )
        self.tool = tool
        self.what = what
        self.vars = ["tool", "what"]

    def do(self, me):
        return Operation("imaginary", Entity(description=self.desc))


############################ CUT TREES TASK #############################

class CutSomething(Task):
    """Use a tool to cut down a tree."""

    def do(self, me):
        if (self.what in me.things) == 0: return
        if (self.tool in me.things) == 0: return
        if not hasattr(self, 'wield') or not self.wield:
            tool = me.find_thing(self.tool)[0]
            self.wield = True
            return Operation("wield", Entity(tool.id))
        what = me.find_thing(self.what)[0]
        return Operation("use", Entity(what.id))


############################ SPOT SOMETHING GOAL ########################

class SpotSomething(Goal):
    """Pick out something and focus on it, forgetting things previously focused on after a while."""

    def __init__(self, what="", range=30, condition=lambda me, a: True, seconds_until_forgotten=30):
        Goal.__init__(self, desc="spot a thing",
                      fulfilled=self.do_i_have,
                      sub_goals=[self.do])
        if isinstance(what, str):
            self.what = what
        else:
            self.what = str(what)

        self.filter = entity_filter.Filter(what)

        self.range = range
        self.condition = condition
        # Keep track of when we've spotted things, so that we can ignore them
        # seconds_until_forgotten is used to forget about spotted things. That way we can re-discover things
        # we've previously focused on after a while.
        self.spotted = {}
        # How many seconds until we've forgotten something we've previously focused on.
        self.seconds_until_forgotten = seconds_until_forgotten
        self.vars = ["what", "range", "seconds_until_forgotten"]

    def do_i_have(self, me):
        something = me.get_knowledge('focus', self.what)
        if something:
            if me.map.get(something) is None:
                me.remove_knowledge('focus', self.what)
            else:
                # Update the time since we last knew about the thing, as we're still actively know about it
                if self.seconds_until_forgotten > 0:
                    self.spotted[something] = time.time()
                return True

    def do(self, me):
        thing_all = me.map.find_by_filter(self.filter)
        nearest = None
        nearest_distance = self.range
        for thing in thing_all:
            # Check that it's not something we've already spotted
            if thing.id in self.spotted:
                # Have we forgotten about that we remembered it yet?
                if time.time() - self.spotted[thing.id] > self.seconds_until_forgotten:
                    self.spotted.pop(thing.id)
                else:
                    continue

            dist = me.steering.distance_to(thing, ai.EDGE, ai.EDGE)
            # FIXME We need a more sophisticated check for parent. Perhaps just
            # check its not in a persons inventory? Requires the ability to
            # do decent type checks
            if dist is not None and dist < self.range and thing.parent and me.entity.parent:
                if self.condition(me, thing):
                    nearest = thing
                    nearest_distance = dist
        if nearest:
            print("Spotted new thing matching '{}' at distance of {:.2f}m. Thing: {}".format(self.what, nearest_distance, str(nearest)))
            me.add_knowledge('focus', self.what, nearest.id)
            # We should only remember things if we can keep them in memory.
            if self.seconds_until_forgotten > 0:
                self.spotted[nearest.id] = time.time()


class SpotSomethingInArea(Goal):
    """Pick out something in a specified area and focus on it, forgetting things previously focused on after a while."""

    def __init__(self, what, location, range=30, condition=lambda me, a: True, seconds_until_forgotten=30):
        Goal.__init__(self,
                      desc="spot a thing in area",
                      fulfilled=self.do_i_have,
                      sub_goals=[self.do])
        self.range = range
        self.area_location_name = location
        self.condition = condition
        self.inner_spot_goal = SpotSomething(what, range=range, seconds_until_forgotten=seconds_until_forgotten,
                                             condition=self.is_thing_in_area)
        self.current_location = None

    def do_i_have(self, me):
        self.inner_spot_goal.do_i_have(me)

    def do(self, me):
        # For performance reasons we'll cache the current location, so we don't have to look it up for each call to is_thing_in_area
        self.current_location = me.get_knowledge("location", self.area_location_name)
        self.inner_spot_goal.do(me)

    def is_thing_in_area(self, me, thing):
        if self.condition(me, thing):
            if self.current_location is None:
                return False
            dist = me.steering.distance_to(thing, ai.EDGE, ai.EDGE)
            if dist is not None and dist < self.range:
                return True
        return False


############################ FETCH SOMETHING GOAL ########################

class FetchSomething(Goal):
    """Get something that is freely available from a known location."""

    def __init__(self, what, where):
        Goal.__init__(self, "fetch a thing",
                      self.is_it_in_my_inventory,
                      [MoveMeArea(where, 20),
                       SpotSomething(what),
                       PickUpFocus(what)])
        self.what = what
        self.vars = ["what"]

    def is_it_in_my_inventory(self, me):
        if self.what in me.things:
            if me.things[self.what][0].parent == me:
                me.remove_knowledge('focus', self.what)
                return 1
        return 0


############################ MOVE SOMETHING GOAL ########################

class TransportSomething(Goal):
    """Move things of a given type from one location to another."""

    def __init__(self, what, src, dest):
        Goal.__init__(self, "move thing to place",
                      false,
                      [FetchSomething(what, src),
                       MoveMe(dest),
                       MoveItOutOfMe(what)])
        self.what = what
        self.vars = ["what"]


############################### SIT DOWN ######################################

class SitDown(Goal):
    """Sit down and rest in a given location."""

    def __init__(self, where):
        Goal.__init__(self, "sit down",
                      self.am_i_sat,
                      [SpotSomething(where),
                       MoveMeToFocus(where),
                       self.sit])

    def am_i_sat(self, me):
        return me.entity.props.mode == "sitting"

    def sit(self, me):
        return Operation("set", Entity(me.entity.id, mode="sitting"))


############################ MOVE AROUND RANDOMLY ###########################################

class Amble(Goal):
    """Move randomly if not already moving."""

    def __init__(self):
        Goal.__init__(self, "move in a random direction if not already in motion",
                      self.am_i_moving,
                      [self.do_amble])
        self.what = "world"

    def am_i_moving(self, me):
        if me.entity.location.velocity:
            return 0
        return 1

    def do_amble(self, me):
        id = me.get_knowledge('focus', 'hook')
        if id is not None:
            thing = me.map.get(id)
            if thing is None:
                me.remove_knowledge('focus', self.what)
            else:
                if thing.parent.id != me.entity.parent.id:
                    me.remove_knowledge('focus', self.what)
                else:
                    if thing.parent.id == me.entity.id:
                        return
        # world =
        # ground = world.id
        # op = Operation("eat", ground)
        # print "Fish ambling"
        target = Location(me.entity.parent, me.entity.location.pos)
        target.pos = Vector3D(target.pos.x + uniform(-1.5, 1.5), target.pos.y, target.pos.z + uniform(-1.5, 1.5))
        target.velocity = Vector3D(1, 0, 0)
        return Operation("move", Entity(me.entity.id, location=target))


############################ FEED (FOR FOOD) ##################################

class Feed(Goal):
    """Eat something."""

    def eat(self, me):
        me.remove_knowledge('focus', self.what)
        if (self.what in me.things) == 0:
            return
        food = me.find_thing(self.what)[0]
        ent = Entity(food.id)
        return Operation("eat", ent)

    def am_i_full(self, me):
        # if hasattr(me, "food") and hasattr(me, "mass"):
        #     if me.food > (self.full * me.mass):
        #         return True
        #     if me.food < (self.full * me.mass / 4):
        #         self.sub_goals[0].range = self.range * 2
        #     else:
        #         self.sub_goals[0].range = self.range
        return False


########################## BUY AND EAT A MEAL #################################

class Meal(Feed):
    """Buy a meal from a given location and eat it."""

    def __init__(self, what, place, seat=None):
        if seat is None:
            Goal.__init__(self, "have a meal",
                          self.am_i_full,
                          [MoveMeArea(place, 10),
                           AcquireThing(what),
                           self.eat])
        else:
            Goal.__init__(self, "have a meal",
                          self.am_i_full,
                          [MoveMeArea(place, 10),
                           AcquireThing(what),
                           SitDown(seat),
                           self.eat])
        self.what = what
        self.place = place
        self.full = 0.01
        self.vars = ["what", "place"]

    def imaginary(self, me):
        return Operation("imaginary", Entity(description=self.desc))


############################ FORAGE (FOR FOOD) ################################

class Forage(Feed):
    """Find food available in a given location and eat it."""

    def __init__(self, what):
        Goal.__init__(self, "forage for food by name",
                      self.am_i_full,
                      [SpotSomething(what, range=5),
                       PickUpFocus(what),
                       self.eat])
        self.what = what
        self.range = 5
        self.full = 0.2
        self.vars = ["what", "range"]


############################ PECK ###########################################

class Graze(Feed):
    """Graze food from the ground at interval."""

    def __init__(self):
        Goal.__init__(self,
                      desc="graze from ground at interval",
                      fulfilled=self.am_i_full_or_grazing,
                      sub_goals=[self.continue_task, self.do_peck],
                      validity=self.have_i_consume_usage)
        self.what = "land"
        self.last_time = None
        self.full = 0.2
        self.interval = 10
        self.vars = ["full", "last_time", "interval"]

    def have_i_consume_usage(self, me):
        usages = me.entity.get_prop_map("_usages")
        return usages and "consume" in usages

    def am_i_full_or_grazing(self, me):
        consume_task = get_task(me, "consume")
        if consume_task:
            return False
        return self.am_i_full(me)

    def continue_task(self, me):
        consume_task = get_task(me, "consume")
        if consume_task:
            return True

    def do_peck(self, me):
        if not self.last_time or time.time() - self.last_time > self.interval:
            self.last_time = time.time()
            # stop moving
            me.steering.set_destination()
            return Operation("use", Operation("consume",
                                              Entity(me.entity.id, targets=[Entity(me.entity.parent.id, pos=me.entity.location.pos)])))


############################ BROWSE (FIND FOOD, EAT SOME, MOVE ON) ###########

class Browse(Feed):
    """Browse for food in a given location."""

    def __init__(self, what, min_status):
        Goal.__init__(self, "browse for food by name",
                      self.am_i_full,
                      [SpotSomething(what, range=20, condition=(
                          lambda me, o, s=min_status: hasattr(o.props, "status") and o.props.status > s)),
                       MoveMeToFocus(what),
                       self.eat])
        self.what = what
        self.range = 20
        self.full = 0.2
        self.min_status = min_status
        self.vars = ["what", "range", "min_status"]

    def eat(self, me):
        if (self.what in me.things) == 0: return
        food = me.find_thing(self.what)[0]
        if food.props.status < self.min_status:
            me.remove_thing(food)
        ent = Entity(food.id)
        return Operation("eat", ent)


############################ PREDATOR (HUNT SOMETHING, THEN EAT IT) ###########

class Predate(Feed):
    """Hunt for a specified type of prey and eat it."""

    def __init__(self, what, range):
        Goal.__init__(self, "predate something",
                      self.am_i_full,
                      [SpotSomething(what, range=range),
                       HuntFor(what, range),
                       self.eat])
        self.what = what
        self.range = range
        self.full = 0.1
        self.vars = ["what", "range"]


class PredateSmall(Feed):
    """Hunt for a specified type of prey with a small mass and eat it."""

    def __init__(self, what, range, max_mass):
        Goal.__init__(self, "predate something",
                      self.am_i_full,
                      [SpotSomething(what, range, condition=(lambda me, o, m=max_mass: hasattr(o, "mass") and o.mass < m)),
                       HuntFor(what, range),
                       self.eat])
        self.what = what
        self.range = range
        self.max_mass = max_mass
        self.full = 0.1
        self.vars = ["what", "range"]


################### HUNT (SEARCH FOR SOMETHING, THEN KILL IT) #################

class Hunt(Goal):
    """Hunt for something, then kill it with a weapon."""

    def __init__(self, weapon, what, range):
        Goal.__init__(self, "hunt something",
                      self.none_in_range,
                      [SpotSomething(what, range),
                       AcquireThing(weapon),
                       HuntFor(what, range, 7),
                       self.fight])
        self.weapon = weapon
        self.what = what
        self.filter = entity_filter.Filter(what)
        self.range = range
        self.vars = ["weapon", "what", "range"]

    def none_in_range(self, me):
        thing_all = me.map.find_by_filter(self.filter)
        for thing in thing_all:
            distance = me.steering.distance_to(thing, ai.EDGE, ai.EDGE)
            if distance and distance < self.range:
                return 0
        return 1

    def fight(self, me):
        if (self.weapon in me.things) == 0:
            print("no weapon")
            return
        weapon = me.find_thing(self.weapon)[0]
        if (weapon.ammo in me.things) == 0:
            print("no ammo")
            return
        ammo = me.find_thing(weapon.ammo)[0]
        if (self.what in me.things) == 0:
            print("no target")
            return
        enemy = me.find_thing(self.what)[0]
        me.remove_thing(ammo)
        return Operation("shoot", Entity(ammo.id), Entity(enemy.id), to=weapon)


##################### DEFEND (SPOT SOMETHING, THEN KILL IT) ###################

class Defend(Goal):
    """Kill something if we see it.
    DEPRECATED
    """

    # def __init__(self, what, range):
    #     Goal.__init__(self, "defend against something",
    #                   self.none_in_sight,
    #                   [spot_something(what),
    #                    hunt_for(what, range),
    #                    self.fight])
    #     self.what = what
    #     self.filter = entity_filter.Filter(what)
    #     self.range = range
    #     self.vars = ["what", "range"]
    #
    # def none_in_sight(self, me):
    #     target_id = me.get_knowledge('focus', self.what)
    #     if target_id:
    #         return 0
    #     thing_all = me.map.find_by_filter(self.filter)
    #     if thing_all and len(thing_all) > 0:
    #         return 0
    #     return 1
    #
    # def fight(self, me):
    #     target_id = me.get_knowledge('focus', self.what)
    #     if not target_id:
    #         return
    #     return Operation("attack", Entity(target_id))


############################ BUY SOMETHING (AS TRADER) ########################

class BuyTrade(GetThing):
    """Go to a location and offer to buy a certain thing."""

    def __init__(self, what, where, when=None):
        Goal.__init__(self, "buy from the public",
                      self.do_I_have_it,
                      [MoveMe(where),
                       self.announce_trade],
                      when)
        self.what = what
        self.ticks = 0
        self.vars = ["what"]

    def announce_trade(self, me):
        self.ticks = self.ticks + 1
        if self.ticks == 5:
            self.ticks = 0
            es = Entity(say="Sell your " + self.what + " here!")
            return Operation("talk", es)


############################ SELL SOMETHING (AS TRADER) ########################

class SellTrade(Goal):
    """Go to a location and off to sell a certain thing."""

    def __init__(self, what, where, when=None):
        Goal.__init__(self, "Sell to the public",
                      self.dont_I_have_it,
                      [MoveMe(where),
                       self.announce_trade],
                      when)
        self.what = what
        self.ticks = 0
        self.vars = ["what"]

    def dont_I_have_it(self, me):
        if self.what in me.things:
            return randint(0, 1)
        else:
            return 1

    def announce_trade(self, me):
        self.ticks = self.ticks + 1
        ret = Oplist()
        if self.ticks == 5:
            self.ticks = 0
            es = Entity(say="Get your " + self.what + " here!")
            ret = ret + Operation("talk", es)
            ret = ret + Operation("imaginary", Entity("shout"))
        else:
            if randint(0, 4) == 1:
                ret = ret + Operation("imaginary", Entity("wave"))
        return ret


################ TRADE (BUY SOMETHING, USE TOOL, SELL PRODUCT) ################

class Trade(Goal):
    """Offer to buy something, then use a tool on it, and sell the product."""

    def __init__(self, wbuy, tool, wsell, where, when=None):
        Goal.__init__(self, "trade at a market",
                      false,
                      [AcquireThing(tool),
                       BuyTrade(wbuy, where),
                       self.process],
                      when)
        self.wbuy = wbuy
        self.tool = tool
        self.wsell = wsell
        self.wield = False
        self.vars = ["wbuy", "tool", "wsell"]

    def process(self, me):
        if (self.wbuy in me.things) == 0: return
        if (self.tool in me.things) == 0: return
        tool = me.find_thing(self.tool)[0]
        if not self.wield:
            self.wield = True
            return Operation("wield", Entity(tool.id))
        thing = me.find_thing(self.wbuy)[0]
        return Operation("use", Entity(thing.id, objtype="obj"))


############################# RUN MARKET STALL ##############################

class Market(Goal):
    """Base class to run a market stall."""

    def is_it(self, me):
        if (self.shop in me.things) == 0: return 0
        shop = me.find_thing(self.shop)[0]
        return shop.mode == self.state

    def set_it(self, me):
        if (self.shop in me.things) == 0: return
        shop = me.find_thing(self.shop)[0]
        return Operation("set", Entity(shop.id, mode=self.state))


class RunShop(Market):
    """Run a market stall."""

    def __init__(self, shop, updown):
        Goal.__init__(self, "run a shop",
                      self.is_it,
                      [SpotSomething(shop),
                       self.set_it])
        # FIXME This probably does not work, but I'll fix it when we need it
        self.shop = shop
        self.state = updown
        self.vars = ["shop"]


######################### KEEP (Things that I own in place) #################

class Keep(Goal):
    """Make sure everything we own of a given type is kept in a place."""

    def __init__(self, what, where):
        Goal.__init__(self, "Keep " + what + " in " + where + ".",
                      self.are_they_there,
                      [self.keep_it])
        self.what = what
        self.where = where
        self.vars = ["what", "where"]

    def are_they_there(self, me):
        if (self.where in me.things) == 0: return 1
        if (self.what in me.things) == 0: return 1
        thing_all = me.find_thing(self.what)
        where = me.find_thing(self.where)[0]
        for thing in thing_all:
            if thing.parent.id != where.id and thing.parent.id != me.entity.id:
                return 0
        return 1

    def keep_it(self, me):
        result = Oplist()
        if (self.where in me.things) == 0: return
        if (self.what in me.things) == 0: return
        thing_all = me.find_thing(self.what)
        where = me.find_thing(self.where)[0]
        to_location = Location(where, Point3D(0, 0, 0))
        minx = where.location.bbox.low_corner.x
        minz = where.location.bbox.low_corner.z
        maxx = where.location.bbox.high_corner.x
        maxz = where.location.bbox.high_corner.z
        for thing in thing_all:
            if thing.parent.id != where.id and thing.parent.id != me.entity.id:
                thingloc = Location(where, Point3D(uniform(minx, maxx), 0, uniform(minz, maxz)))
                result.append(Operation("move", Entity(thing.id, location=thingloc)))
        return result


################## KEEP ON ME (money kept on my person) ################

class KeepOnMe(Goal):
    """Make sure everything we own of a given type is in inventory."""

    def __init__(self, what):
        Goal.__init__(self, "Keep " + what + " on me.",
                      self.are_they_on_me,
                      [self.keep_it])
        self.what = what
        self.vars = ["what"]

    def are_they_on_me(self, me):
        thing_all = me.find_thing(self.what)
        for thing in thing_all:
            if thing.parent.id != me.entity.id:
                return 0
        return 1

    def keep_it(self, me):
        result = Oplist()
        if (self.what in me.things) == 0: return
        thing_all = me.find_thing(self.what)
        to_loc = Location(me, Point3D(0, 0, 0))
        for thing in thing_all:
            if thing.parent.id != me.entity.id:
                result.append(Operation("move", Entity(thing.id, location=to_loc)))
        return result


################## ASSEMBLE (Thing from list of parts) #################

class Assemble(Goal):
    """Assemble something from a list of parts."""

    def __init__(self, me, what, fromwhat):
        sgoals = []
        for item in fromwhat:
            sgoals.append(SpotSomething(item))
            sgoals.append(MoveIt(item, me.entity.location, 1))
        sgoals.append(self.build)
        Goal.__init__(self, "Build " + what + " from parts.",
                      false, sgoals)
        self.what = what
        self.fromwhat = fromwhat
        self.vars = ["what", "fromwhat"]

    def build(self, me):
        retops = Oplist()
        for item in self.fromwhat:
            if (item in me.things) == 0: return
        for item in self.fromwhat:
            cmpnt = me.find_thing(item)[0]
            retops = retops + Operation("set", Entity(cmpnt.id, status=-1))
        retops = retops + Operation("create",
                                    Entity(name=self.what, parent=self.what, location=me.entity.location.copy()))
        retops = retops + Operation("imaginary", Entity("conjure"))
        return retops


######################## TRANSACTION (Sell thing) #######################

class Transaction(Goal):
    """Conduct a transaction."""

    def __init__(self, what, who, cost):
        Goal.__init__(self, "conduct transaction",
                      self.transaction_inactive,
                      [self.transact])
        self.what = what
        self.who = who
        self.cost = int(cost)
        self.payed = 0
        self.vars = ["what", "who", "cost", "payed"]

    def check_availability(self, me):
        return (self.what in me.things) == 0

    def transaction_inactive(self, me):
        if self.check_availability(me):
            return 0
        if len(me.money_transfers) == 0:
            return 1
        payed = self.payed
        if payed == -1: payed = 0
        for item in me.money_transfers:
            if item[0] == self.who:
                payed = payed + int(item[1])
        for item in me.money_transfers:
            if item[0] == self.who:
                me.money_transfers.remove(item)
        if payed != self.payed:
            self.payed = payed
            return 0
        return 1

    def transact(self, me):
        who = me.map.get(self.who)
        if (self.what in me.things) == 0:
            self.irrelevant = 1
            return Operation("talk", Entity(say="I don't have any " + self.what + " left."))
        if self.payed < self.cost:
            return Operation("talk", Entity(say=who.name + " you owe me " + str(self.cost - self.payed) + " coins."))
        thing = me.find_thing(self.what)[0]
        res = Oplist()
        me.remove_thing(thing)
        res.append(Operation("move", Entity(thing.id, location=Location(who, Point3D(0, 0, 0)))))
        res.append(Operation("talk", Entity(say="Thankyou for your custom.")))
        self.irrelevant = 1
        return res


class HirelingTransaction(Transaction):
    """Conduct a transaction to hire our services."""

    def check_availability(self, me):
        return me.get_knowledge('employer', me) != None

    def transact(self, me):
        employer = me.get_knowledge('employer', me)
        if employer:
            print('Already employed by ' + employer)
            return Operation("talk", Entity(say="Sorry, I am currently working for someone else."))
        who = me.map.get(self.who)
        if not who:
            print("Who am I talking to")
            return
        if self.payed < self.cost:
            return Operation("talk", Entity(say=who.name + " you owe me " + str(self.cost - self.payed) + " coins."))
        res = Oplist()
        me.add_knowledge('employer', me.entity.id, who.id)
        # FIXME add the new goal
        goal = Accompany(who.id)
        me.goals.insert(0, goal)
        res.append(Operation("talk", Entity(say="I will help you out until sundown today.")))
        self.irrelevant = 1
        return res


######################## ACTUATION (Operate device) #######################

# class activate_device(Goal):
#     """Look for something and activate it."""
#
#     def __init__(self, what):
#         Goal.__init__(self, "activate a thing",
#                       self.activated,
#                       [spot_something(what), self.activate_focus])
#         self.what = what
#         self.vars = ["what"]
#
#     def activated(self, me):
#         return False
#
#     def activate_focus(self, me):
#         print("Activating ", self.what)
#         something = me.get_knowledge('focus', self.what)
#         assert (something)
#         if me.map.get(something) == None:
#             me.remove_knowledge('focus', self.what)
#             return
#         return Operation('actuate', Operation('chop', Entity(something)))


class ClearFocus(Goal):
    """Clear whatever is in the current focus."""

    def __init__(self, what):
        Goal.__init__(self, "clear focus",
                      self.clear_focus,
                      [])
        if isinstance(what, list):
            self.what = what
        else:
            self.what = [what]
        self.vars = ["what"]

    def clear_focus(self, me):
        for what in self.what:
            something = me.get_knowledge('focus', what)
            if something:
                me.remove_knowledge('focus', what)
        return 1


######################## Linger (Wait for a certain time) #######################

class Linger(Goal):
    """Linger for a period of time.
    The amount of ticks to wait is randomized as between minTicks and maxTicks.
    When active, the tickCounter will be decreased and an empty Oplist returned.
    Once waiting completes, the Goal reports itself as fulfilled, only to reset the
    tickCounter for next iteration.
    """

    def __init__(self, min_ticks=0, max_ticks=0):
        Goal.__init__(self, "linger awhile",
                      self.activated,
                      [self.wait])
        self.minTicks = min_ticks
        if max_ticks:
            self.maxTicks = max_ticks
        else:
            self.maxTicks = min_ticks
        self.tickCounter = randint(self.minTicks, self.maxTicks)
        self.vars = ["minTicks", "maxTicks", "tickCounter"]

    def activated(self, me):
        if self.tickCounter <= 0:
            self.tickCounter = randint(self.minTicks, self.maxTicks)
            return True
        self.tickCounter = self.tickCounter - 1
        return False

    def wait(self, me):
        return Oplist()


class Iterate(Goal):
    """Iterate through the goals, moving to next if any is fulfilled"""

    def __init__(self, goals):
        Goal.__init__(self, "iterate over goals",
                      self.check_subgoal,
                      goals)

    def check_subgoal(self, me):
        first_goal = self.sub_goals[0]
        if first_goal.is_fulfilled:
            self.sub_goals.pop(0)
            self.sub_goals.push(first_goal)
