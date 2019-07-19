# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

from atlas import Operation, Entity, Oplist

from mind.goals.dynamic.DynamicGoal import DynamicGoal
from physics import square_distance, distance_between
from rules import Location
from mind.Goal import Goal
from mind.goals.common.move import avoid, move_me_to_focus
from mind.goals.common.misc_goal import spot_something
import entity_filter

import time
import types


class Fight(Goal):
    """Fight enemies in range"""

    # TODO: Make entity first select weapon, and then adjust strategy depending on weapon.
    # TODO: I.e. when using a ranged weapon the entity should keep range.

    def __init__(self, what="", range=30):
        Goal.__init__(self, "fight something",
                      self.none_in_range,
                      [spot_something(what=what, range=range),
                       self.equip_weapon,
                       self.attack,
                       move_me_to_focus(what=what, radius=0, speed=0.5),
                       #                       hunt_for(what=what, range=range, proximity=3),
                       ])
        self.what = what
        self.filter = entity_filter.Filter(what)
        self.range = range
        self.square_range = range * range
        self.vars = ["what", "range"]

    def none_in_range(self, me):
        thing_all = me.map.find_by_filter(self.filter)
        for thing in thing_all:
            if square_distance(me.entity.location, thing.location) < self.square_range:
                return 0
        return 1

    def equip_weapon(self, me):
        # First check if we're holding a weapon
        attached_current = me.get_attached_entity("hand_primary")
        has_attached = False
        if attached_current:
            has_attached = True
            # Check that the attached entity can be used to strike
            usages = attached_current.get_prop_map("usages")
            if usages and 'strike' in usages:
                    return None
        # Current tool isn't a weapon, or we have nothing attached, try to find one, and if not unequip so we can fight with our fists
        for child in me.entity.contains:
            usages = child.get_prop_map("usages")
            if usages and 'strike' in usages:
                return Operation("wield", Entity(child.id, attachment="hand_primary"))

        # Couldn't find any weapon to wield, check if we should unwield the current tool so we can fight with our fists
        if has_attached:
            return Operation("wield", Entity(attachment="hand_primary"))

        return None

    def get_reach(self, me):

        reach = 0
        own_reach = me.entity.get_prop_float('reach')
        if own_reach:
            reach += own_reach

        attached_current = me.get_attached_entity("hand_primary")
        if attached_current:
            attached_reach = attached_current.get_prop_float('reach')
            if attached_reach:
                reach += attached_reach

        return reach

    def attack(self, me):

        target_id = me.get_knowledge('focus', self.what)
        if target_id is None:
            print("No focus target")
            return
        enemy = me.map.get(target_id)
        if enemy is None:
            print("No target")
            me.remove_knowledge('focus', self.what)
            return

        # check that we can reach the target, and if so attack it
        distance = distance_between(me.entity.location, enemy.location)
        if distance is None:
            print("Could not calculate distance.")
            return
        reach = self.get_reach(me)
        if distance - reach <= 0:
            attached_current = me.get_attached_entity("hand_primary")
            if attached_current:
                print("Striking")
                return Operation("use", Operation("strike", Entity(attached_current.id, targets=[Entity(enemy.id)])))
            else:
                print("Punching")
                return Operation("use", Operation("punch", Entity(me.entity.id, targets=[Entity(enemy.id)])))
        else:
            print("Out of reach. Reach is {} and distance is {}".format(reach, distance))


class FightOrFlight(Goal):
    """Fight or flee from enemies in range"""

    def __init__(self, range=30):
        Goal.__init__(self, desc="fight or flee from enemies",
                      subgoals=[Fight(what="memory.disposition <= -1 && memory.threat < 0", range=range),
                                avoid(what="memory.disposition <= -1 && memory.threat > 0", range=range)])


class KeepGrudge(DynamicGoal):
    """React to other entities hitting us and lower their disposition."""

    def __init__(self):
        DynamicGoal.__init__(self, trigger="sight_hit", desc="React when being hit.")

    def event(self, me, original_op, op):
        print('Got sight of hit')
        ent = op[0]
        if ent:
            from_id = ent["from"]
            to_id = ent["to"]
            if from_id and to_id:
                # Check that it's not from ourselves
                if from_id == me.entity.id:
                    return
                # Ignore it it's us being hit
                if to_id != me.entity.id:
                    return

                # Alter the base disposition for this entity
                disposition_base = me.map.recall_entity_memory(from_id, "disposition_base", 0)
                # Alter it slightly. Here we could check how much damage it did and alter it depending on that
                me.map.add_entity_memory(from_id, "disposition_base", disposition_base - 0.4)
                print("Updated base disposition to {}.".format(disposition_base - 0.4))
                entity = me.entities[from_id]
                if entity:
                    me.update_relation_for_entity(entity)
