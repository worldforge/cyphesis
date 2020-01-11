# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import Operation, Entity, Oplist
from physics import Point3D
from rules import Location

import mind.goals
import mind.goals.common
from mind.Goal import Goal
from mind.goals.common.common import *
from mind.goals.common.misc_goal import HirelingTransaction
from mind.goals.dynamic.DynamicGoal import DynamicGoal


# class sell_things(add_unique_goal_by_perception):
# def __init__(self, desc="add transaction goal"):
# add_unique_goal_by_perception.__init__(self,
# transaction,
# trigger="sound_talk",
# desc=desc)

class HireTrade(DynamicGoal):
    """Respond to a request for help."""

    def __init__(self, desc="be available for hire"):
        DynamicGoal.__init__(self,
                             trigger="interlinguish_desire_verb3_hire_verb1",
                             desc=desc)

    def event(self, me, op, say):
        # print "To got hired"
        price = me.get_knowledge('price', 'services')
        if not price:
            print("No price")
            return
        # print "I go for " + str(price) + " coins"
        goal = HirelingTransaction('services', op.to, price)
        me.goals.insert(0, goal)
        return Operation("talk", Entity(say=me.map.get(op.to).name + " one day will be " + str(price) + " coins"))


class BuyFrom(Goal):
    """Respond to an offer to sell something."""

    def __init__(self, what, cost, who, desc="buy livestock from someone"):
        Goal.__init__(self, desc, false,
                      [self.check])
        self.what = what
        self.cost = cost
        self.who = who
        self.last_valued = None
        self.vars = ["what", "cost", "who", "last_valued"]

    def check(self, me):
        seller = me.map.get(self.who)
        if not seller:
            return
        for transfer in me.transfers:
            if transfer[0] != self.who:
                continue
            item = me.map.get(transfer[1])
            if not item:
                continue
            if item.type[0] != self.what:
                continue
            # FIXME Removing messes up the for loop
            me.transfers.remove(transfer)
            if item.mass:
                price = int(item.mass * self.cost)
                coins = me.find_thing("coin")
                if len(coins) < price:
                    me.remove_thing(item)
                    return Operation("talk", Entity(
                        say="I can't afford to buy that " + self.what + " at the moment.")) + Operation("move",
                                                                                                        Entity(item.id,
                                                                                                               location=Location(
                                                                                                                   seller.id,
                                                                                                                   Point3D(
                                                                                                                       0,
                                                                                                                       0,
                                                                                                                       0))))
                res = Oplist()
                for i in range(0, price):
                    coin = coins[0]
                    me.remove_thing(coin)
                    res.append(Operation("move", Entity(coin.id, location=Location(seller, Point3D(0, 0, 0)))))
                res.append(Operation("talk", Entity(
                    say="Thankyou " + seller.name + ", here are " + str(price) + " coins for the pig.")))
                self.irrelevant = 1
                return res

        if not hasattr(seller, "right_hand_wield") or not seller.right_hand_wield:
            return
        if self.last_valued and seller.right_hand_wield == self.last_valued:
            return
        wield = me.map.get(seller.right_hand_wield)
        if not wield:
            return
        if self.what != wield.type[0]:
            return
        if not wield.mass:
            return
        price = int(wield.mass * self.cost)
        coins = me.find_thing("coin")
        self.last_valued = wield.id
        if len(coins) < price:
            return Operation("talk", Entity(say="I can't afford to buy that " + self.what + " at the moment."))
        else:
            return Operation("talk",
                             Entity(say=seller.name + " that " + self.what + " is worth " + str(price) + " coins."))


class BuyLivestock(DynamicGoal):
    """Respond to an offer to sell livestock by the kg."""

    def __init__(self, what, cost, desc="buy livestock by the kg"):
        DynamicGoal.__init__(self,
                             trigger="interlinguish_desire_verb3_sell_verb1",
                             desc=desc)
        self.cost = int(cost)
        self.what = what

    def event(self, me, op, say):
        object = say[1].word
        thing = me.map.get(object)
        who = me.map.get(op.to)
        if thing is None:
            if object != self.what:
                return Operation("talk",
                                 Entity(say=who.name + ", I am not interested in buying your " + str(object) + "."))
            me.goals.insert(0, BuyFrom(self.what, self.cost, op.to))
            return Operation("talk", Entity(say=who.name + " which " + object + " would you like to sell?"))
        if self.what not in thing.type:
            return
        if thing in me.find_thing(self.what):
            return
        # price=me.get_knowledge("price", thing.type[0])
        price = self.cost * int(thing.mass)
        res = Oplist()
        coins = me.find_thing("coin")
        if len(coins) < int(price):
            print("Coins: " + str(len(coins)) + " Cost: " + str(self.cost))
            return Operation("talk", Entity(say="I can't afford any " + self.what + "s at the moment."))
        for i in range(0, int(price)):
            coin = coins[0]
            me.remove_thing(coin)
            res.append(Operation("move", Entity(coin.id, location=Location(who, Point3D(0, 0, 0)))))
        res.append(Operation("talk", Entity(say="Thankyou " + who.name + ", come again.")))
        me.add_thing(thing)
        return res
