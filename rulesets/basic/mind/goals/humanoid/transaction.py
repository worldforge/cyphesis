#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

import mind.goals
import mind.goals.common
from mind.goals.common.common import *
from mind.goals.common.move import move_me
from mind.goals.dynamic.DynamicGoal import DynamicGoal
from mind.goals.dynamic.add_unique_goal import add_unique_goal_by_perception
from Point3D import Point3D

#class sell_things(add_unique_goal_by_perception):
    #def __init__(self, desc="add transaction goal"):
        #add_unique_goal_by_perception.__init__(self,
                                               #transaction,
                                               #trigger="sound_talk",
                                               #desc=desc)

class hire_trade(DynamicGoal):
    def __init__(self, desc="be available for hire"):
        DynamicGoal.__init__(self,
                             trigger="interlinguish_desire_verb3_hire_verb1",
                             desc=desc)
    def event(self, me, op, say):
        #print "To got hired"
        price = me.get_knowledge('price', 'services')
        if not price:
            print "No price"
            return
        #print "I go for " + str(price) + " coins"
        goal = mind.goals.common.misc_goal.hireling_transaction('services', op.from_, price)
        me.goals.insert(0,goal)
        return Operation("talk", Entity(say=op.from_.name+" one day will be "+str(price)+" coins"))

class buy_livestock(DynamicGoal):
    def __init__(self, what, cost, desc="buy livestock by the kg"):
        DynamicGoal.__init__(self,
                             trigger="interlinguish_desire_verb3_sell_verb1",
                             desc=desc)
        self.cost=int(cost)
        self.what=what
    def event(self, me, op, say):
        object=say[1].word
        thing=me.map.get(object)
        if thing==None:
            return Operation("talk", Entity(say=who.name+" which "+object+" would you like to sell?"))
        who=op.from_
        if not self.what in thing.type: return
        if thing in me.find_thing(self.what): return
        #price=me.get_knowledge("price", thing.type[0])
        price=self.cost*int(thing.mass)
        res=Message()
        coins = me.find_thing("coin")
        if len(coins) < int(price):
            print "Coins: " + len(coins) + " Cost: " + self.cost
            return Operation("talk", Entity(say="I can't afford any "+self.what+"s at the moment."))
        for i in range(0, int(price)):
            coin=coins[0]
            me.remove_thing(coin)
            res.append(Operation("move",Entity(coin.id, location=Location(who, Point3D(0,0,0)))))
        res.append(Operation("talk", Entity(say="Thankyou "+who.name+", come again.")))
        me.add_thing(thing)
        return res
