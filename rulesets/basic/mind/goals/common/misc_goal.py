#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
#Al Riddoch - Added transport_something() goal

from mind.goals.common.common import *
from mind.goals.common.move import *
from whrandom import *

########################## MAKE LOTS OF SOMETHING #################################

class make_amount(Goal):
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

############################ NOT YET IMPLEMENTED ####################################

class nyi(Goal):
    def __init__(self, desc):
        Goal.__init__(self,desc,
                      lambda self:0,
                      [self.not_yet_implemented])
        self.vars=["desc"]
    def not_yet_implemented(self, me):
        #CHEAT! (make it so that it asks other people for knowledge)
        return Operation("NYI",Entity(description=self.desc))

############################ IMAGINARY TASK ####################################

class imaginary(Goal):
    def __init__(self, desc, time, place):
        Goal.__init__(self,desc,false,[move_me(place),self.imaginary],time)
        self.vars=["desc"]
    def imaginary(self,me):
        return Operation("imaginary",Entity(description=self.desc))

############################ GET KNOWLEDEGE ABOUT THING #############################

class get_knowledge(Goal):
    def __init__(self, me, what):
        Goal.__init__(self,"get knowledge about thing by name",
                      self.do_I_know_about_this,
                      [nyi("Try another task or try asking around!")])
        self.what=what
        self.vars=["what"]
    def do_I_know_about_this(self, me):
        if me.things.has_key(self.what):
            return 1
        return me.get_knowledge("place",self.what)

############################ BUY KNOWN THING ##############################

class get_thing(Goal):
    def do_I_have_it(self, me):
        return me.things.has_key(self.what)

class buy_thing(get_thing):
    def __init__(self, me, what):
        #CHEAT!: doesn't work for static goals ;-(
        #goal systems needs redesign
        place=me.get_knowledge("place",what)
        Goal.__init__(self,"buy thing by name (I am at shop)",
                      self.do_I_have_it,
                      [move_me(place),
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
    def is_it_in_my_inventory(self,me):
        if me.things.has_key(self.what):
            return me.things[self.what][0].location.parent==me
        return 0

############################ ACQUIRE KNOWN THING ##############################

class acquire_known_thing(acquire):
    def __init__(self, me, what):
        Goal.__init__(self,"acquire known thing by name",
                      self.is_it_in_my_inventory,
                      [buy_thing(me,what),
                       move_it_into_me(me,what)])
        self.what=what
        self.vars=["what"]

############################ ACQUIRE THING ####################################

class acquire_thing(acquire):
    def __init__(self, me, what):
        Goal.__init__(self,"acquire thing by name",
                      self.is_it_in_my_inventory,
                      [get_knowledge(me,what),
                       acquire_known_thing(me,what)])
        self.what=what
        self.vars=["what"]

        
############################ TASK WITH PLACE AND TOOL #########################

class task(Goal):
    def __init__(self, me, desc, time, place, what, tool):
        Goal.__init__(self,desc,
                      false,
                      [acquire_thing(me,tool),
                       move_me_area(place),
                       spot_something(what),
                       move_me(lambda me,what=what:me.things[what][0].location),
                       self.do],
                      time)
        self.tool=tool
        self.what=what
        self.vars=["tool","what"]
    def do(self,me):
        return Operation("imaginary",Entity(description=self.desc))

############################ CUT TREES TASK #############################

class cut_something(task):
    def do(self,me):
        if me.things.has_key(self.tool)==0: return
        obj=me.find_thing(self.tool)[0]
        if me.things.has_key(self.what)==0: return
        what=me.find_thing(self.what)[0]
        return Operation("cut",Entity(obj.id),Entity(what.id),to=obj)

############################ SPOT SOMETHING GOAL ########################

class spot_something(Goal):
    def __init__(self, what, range=30, condition=lambda a:1):
        Goal.__init__(self, "spot a thing",
                      self.do_I_have,
                      [self.do])
        self.what=what
        self.range=range
        self.condition=condition
        self.vars=["what","range"]
    def do_I_have(self, me):
        if me.things.has_key(self.what)==1:
            for thing in me.find_thing(self.what):
                locid = thing.location.parent.id
                if locid!=me.location.parent.id and locid!=me.id:
                    me.remove_thing(thing)
        return me.things.has_key(self.what)==1
    def do(self,me):
        thing_all=me.map.find_by_type(self.what)
        nearest=None
        neardist=self.range
        for thing in thing_all:
            distance = me.get_xyz().distance(thing.get_xyz())
            if distance < neardist and thing.location.parent.id==me.location.parent.id:
                if self.condition(thing):
                    nearest = thing
                    neardist = distance
                else:
                    print "Too much for me"
        if nearest:
            me.add_thing(nearest)
                      
############################ FETCH SOMETHING GOAL ########################

class fetch_something(acquire):
    def __init__(self, me, what, where):
        Goal.__init__(self, "fetch a thing",
                      self.is_it_in_my_inventory,
                      [move_me_area(where, 20),
                       spot_something(what),
                       move_it_into_me(me, what)])
        self.what=what
        self.vars=["what"]

############################ MOVE SOMETHING GOAL ########################

class transport_something(Goal):
    def __init__(self, me, what, src, dest):
        Goal.__init__(self,"move thing to place",
                      false,
                      [fetch_something(me, what, src),
                       move_me(dest),
                       move_it_outof_me(what)])
        self.what=what
        self.vars=["what"]

############################ FEED (FOR FOOD) ##################################

class feed(Goal):
    def eat(self,me):
        if me.things.has_key(self.what)==0: return
        food=me.find_thing(self.what)[0]
        ent=Entity(food.id)
        ent1=Entity(me.id)
        return Operation("eat",ent,ent1,to=food,from_=me)
    def am_i_full(self,me):
        if hasattr(me,"food") and hasattr(me,"weight"):
            if me.food > (self.full * me.weight):
                return 1
            if me.food < (self.full * me.weight / 4):
                self.subgoals[0].range = self.range * 2
            else:
                self.subgoals[0].range = self.range
        return 0

############################ FORAGE (FOR FOOD) ################################

class forage(feed):
    def __init__(self, me, what):
        Goal.__init__(self, "forage for food by name",
                      self.am_i_full,
                      [spot_something(what, range=5),
                       move_it_into_me(me, what),
                       self.eat])
        self.what=what
        self.range=5
        self.full=0.2
        self.vars=["what","range"]
 
############################ PREDATOR (HUNT SOMETHING, THEN EAT IT) ###########

class predate(feed):
    def __init__(self, me, what, range):
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
    def __init__(self, me, what, range, max_weight):
        Goal.__init__(self, "predate something",
                      self.am_i_full,
                      [spot_something(what, range, condition=(lambda o,m=max_weight:hasattr(o,"weight") and o.weight < m)),
                       hunt_for(what, range),
                       self.eat])
        self.what=what
        self.range=range
        self.max_weight=max_weight
        self.full=0.1
        self.vars=["what","range"]

##################### DEFEND (SPOT SOMETHING, THEN KILL IT) ###################

class defend(Goal):
    def __init__(self, me, with, what, range):
        Goal.__init__(self, "defend against something",
                      self.none_in_range,
                      [spot_something(what, range),
                       acquire_thing(me, with),
                       hunt_for(what, range),
                       self.fight])
        self.with=with
        self.what=what
        self.range=range
        self.vars=["with", "what", "range"]
    def none_in_range(self, me):
        thing_all=me.map.find_by_type(self.what)
        for thing in thing_all:
            if me.get_xyz().distance(thing.get_xyz()) < self.range:
                return 0
        return 1
    def fight(self, me):
        if me.things.has_key(self.with)==0: return
        weapon=me.find_thing(self.with)[0]
        if me.things.has_key(self.what)==0: return
        enemy=me.find_thing(self.what)[0]
        return Operation("cut",Entity(weapon.id),Entity(enemy.id),to=weapon)

############################ BUY SOMETHING (AS TRADER) ########################

class buy_trade(get_thing):
    def __init__(self, what, where):
        Goal.__init__(self, "buy from the public",
                      self.do_I_have_it,
                      [move_me(where),
                       self.announce_trade])
        self.what=what
        self.ticks=0
        self.vars=["what"]
    def announce_trade(self,me):
        self.ticks=self.ticks+1
        if self.ticks==5:
            self.ticks=0
            es=Entity(say="Sell your " + self.what + " here!")
            return Operation("talk",es,from_=me,to=me)


############################ SELL SOMETHING (AS TRADER) ########################

class sell_trade(Goal):
    def __init__(self, what, where):
        Goal.__init__(self, "Sell to the public",
                      self.dont_I_have_it,
                      [move_me(where),
                       self.announce_trade])
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
        mode_=me.mode
        ret = Message()
        if self.ticks==5:
            self.ticks=0
            es=Entity(say="Get your " + self.what + " here!")
            ret = ret + Operation("talk",es,from_=me,to=me)
            me.mode="shouting"
        else:
            if me.mode!="standing":
                me.mode="standing"
            else:
                if randint(0,4)==1:
                    me.mode="waving"
        if me.mode!=mode_:
            ret = ret + Operation("set", Entity(me.id, mode=me.mode), to=me)
        return ret

############################ TRADE (BUY SOMETHING, USE TOOL, SELL PRODUCT) ####

class trade(Goal):
    def __init__(self, me, wbuy, tool, op, wsell, where):
        Goal.__init__(self, "trade at a market",
                      false,
                      [acquire_thing(me,tool),
                       buy_trade(wbuy, where),
                       self.process])
        self.wbuy=wbuy
        self.tool=tool
        self.op=op
        self.wsell=wsell
        self.vars=["wbuy","tool","op","wsell"]
    def process(self,me):
        if me.things.has_key(self.wbuy)==0: return
        if me.things.has_key(self.tool)==0: return
        thing=me.find_thing(self.wbuy)[0]
        tool=me.find_thing(self.tool)[0]
        return Operation(self.op,Entity(tool.id),Entity(thing.id),to=tool)

######################### KEEP (Things that I own in place) #################

class keep(Goal):
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
        result=Message()
        if me.things.has_key(self.where)==0: return
        if me.things.has_key(self.what)==0: return
        thing_all=me.find_thing(self.what)
        where=me.find_thing(self.where)[0]
        to_location=Location(where,Vector3D(0,0,0))
        for thing in thing_all:
            if thing.location.parent.id!=where.id and thing.location.parent.id!=me.id:
                if hasattr(where, "length") and hasattr(where, "width"):
                    thingloc=Location(where,Vector3D(uniform(0,where.width),uniform(0,where.length),0))
                else:
                    thingloc=to_location
                result.append(Operation("move",Entity(thing.id, location=thingloc)))
        return result

################## KEEP ON ME (money kept on my person) ################

class keep_on_me(Goal):
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
        result=Message()
        if me.things.has_key(self.what)==0: return
        thing_all=me.find_thing(self.what)
        to_loc=Location(me,Vector3D(0,0,0))
        for thing in thing_all:
            if thing.location.parent.id!=me.id:
                result.append(Operation("move",Entity(thing.id, location=to_loc)))
        return result

################## ASSEMBLE (Thing from list of parts) #################

class assemble(Goal):
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
	retops=Message()
        for item in self.fromwhat:
            if me.things.has_key(item)==0: return
        for item in self.fromwhat:
            cmpnt=me.find_thing(item)[0]
            retops = retops + Operation("set", Entity(cmpnt.id,status=-1), to=cmpnt)
        retops = retops + Operation("create", Entity(name=self.what,parents=[self.what], location=me.location.copy()), to=me)
        return retops

######################## TRANSACTION (Sell thing) #######################

class transaction(Goal):
    def __init__(self, what, who, cost):
        Goal.__init__(self,"conduct transaction",
                      self.transaction_inactive,
                      [self.transact])
        self.what=what
        self.who=who
        self.cost=int(cost)
        self.payed=0
        self.vars=["what", "who", "cost", "payed"]
    def transaction_inactive(self,me):
        if me.things.has_key(self.what)==0:
            return 0
        if len(me.money_transfers)==0:
            return 1
        payed=self.payed
        if payed==-1: payed=0
        for item in me.money_transfers:
            if item[0]==self.who.id:
                payed=payed+int(item[1])
                me.money_transfers.remove(item)
        if payed != self.payed:
            self.payed=payed
            return 0
        return 1
    def transact(self,me):
        if me.things.has_key(self.what)==0:
            self.irrelevant=1
            return Operation("talk",Entity(say="I don't have any "+self.what+" left."))
        if self.payed < self.cost:
            return Operation("talk",Entity(say=self.who.name+" you owe me "+str(self.cost-self.payed)+" coins."))
        thing=me.find_thing(self.what)[0]
        res=Message()
        me.remove_thing(thing)
        res.append(Operation("move",Entity(thing.id, location=Location(self.who,Vector3D(0,0,0))),to=thing))
        res.append(Operation("talk",Entity(say="Thankyou for your custom.")))
        self.irrelevant=1
        return res
