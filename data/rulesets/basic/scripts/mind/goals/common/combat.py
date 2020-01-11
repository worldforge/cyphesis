# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

import entity_filter
from atlas import Operation, Entity, Root
from physics import square_distance, distance_between

from mind.Goal import Goal
from mind.goals.common.misc_goal import SpotSomething
from mind.goals.common.move import Avoid, MoveMeToFocus
from mind.goals.dynamic.DynamicGoal import DynamicGoal

# A list of usages which we should look for in weapons.
weapon_usages = ['strike', 'chop']


class Fight(Goal):
    """Fight enemies in range"""

    # TODO: Make entity first select weapon, and then adjust strategy depending on weapon.
    # TODO: I.e. when using a ranged weapon the entity should keep range.

    def __init__(self, what="", range=30):
        Goal.__init__(self, "fight something",
                      self.none_in_range,
                      [
                          SpotSomething(what=what, range=range),
                          self.equip_weapon,
                          self.equip_shield,
                          self.attack,
                          MoveMeToFocus(what=what, radius=0, speed=0.5),
                          #                       hunt_for(what=what, range=range, proximity=3),
                      ])
        self.what = what
        self.filter = entity_filter.Filter(what)
        self.range = range
        self.square_range = range * range
        self.vars = ["what", "range"]
        self.weapon_usage = None
        self.use_ranged = False

    def none_in_range(self, me):
        thing_all = me.map.find_by_filter(self.filter)
        for thing in thing_all:
            distance = square_distance(me.entity.location, thing.location)
            if distance and distance < self.square_range:
                return False
        return True

    def equip_shield(self, me):
        # First check if we're holding a shield
        attached_current = me.get_attached_entity("hand_secondary")
        has_attached = False
        if attached_current:
            has_attached = True
            # Check that the attached entity is a shield
            if attached_current.is_type("shield"):
                return None
        # Current tool isn't a shield, or we have nothing attached, try to find one.
        for child in me.entity.contains:
            if child.is_type("shield"):
                return Operation("wield", Entity(child.id, attachment="hand_secondary"))
        return None

    def equip_ranged_weapon(self, me):
        # First check if we have any arrows
        has_arrows = False
        for child in me.entity.contains:
            if child.is_type("arrow"):
                has_arrows = True
                break

        if not has_arrows:
            return None

        attached_current = me.get_attached_entity("hand_primary")
        if attached_current:
            # Check that the attached entity can be used to strike
            if attached_current.is_type("bow"):
                self.use_ranged = True
                return None
        # Current tool isn't a bow, or we have nothing attached, try to find one.
        for child in me.entity.contains:
            if child.is_type("bow"):
                self.use_ranged = True
                return Operation("wield", Entity(child.id, attachment="hand_primary"))

    def equip_melee_weapon(self, me):
        # First check if we're holding a weapon
        attached_current = me.get_attached_entity("hand_primary")
        has_attached = False
        if attached_current:
            has_attached = True
            # Check that the attached entity can be used to strike
            usages = attached_current.get_prop_map("usages")
            if usages:
                for usage, _ in usages.items():
                    if usage in weapon_usages:
                        self.weapon_usage = usage
                        return None
        # Current tool isn't a weapon, or we have nothing attached, try to find one,
        # and if not unequip so we can fight with our fists
        for child in me.entity.contains:
            usages = child.get_prop_map("usages")
            if usages:
                for usage, _ in usages.items():
                    if usage in weapon_usages:
                        self.weapon_usage = usage
                        return Operation("wield", Entity(child.id, attachment="hand_primary"))

        # Couldn't find any weapon to wield, check if we should unwield the current tool so we can fight with our fists
        if has_attached:
            return Operation("wield", Entity(attachment="hand_primary"))

        return None

    def equip_weapon(self, me):
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

        # If possible, shoot with ranged weapons if a bit away
        self.use_ranged = False
        if distance > 10:
            res = self.equip_ranged_weapon(me)
            if res:
                return res
        if not self.use_ranged:
            return self.equip_melee_weapon(me)

    @staticmethod
    def get_reach(me):

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

        if self.use_ranged:
            res = self.attack_ranged(me, enemy)
            if self.use_ranged:
                return res
        # If we couldn't use ranged weapons (or we're too close) switch to melee.
        return self.attack_melee(me, enemy)

    def attack_melee(self, me, enemy):
        print("attack melee")
        # check that we can reach the target, and if so attack it
        distance = distance_between(me.entity.location, enemy.location)
        if distance is None:
            print("Could not calculate distance.")
            return
        reach = self.get_reach(me)
        attached_current = me.get_attached_entity("hand_primary")
        tasks_prop = me.entity.get_prop_map('tasks')
        if distance - reach <= 0:
            move_to_face = me.face(enemy)

            if attached_current:
                # Check if we're already hitting
                if tasks_prop:
                    if "melee" in tasks_prop:
                        return True
                print("Hitting with a weapon")
                return move_to_face + Operation("use", Operation(self.weapon_usage, Entity(attached_current.id,
                                                                                           targets=[Entity(enemy.id)])))
            else:
                # Check if we're already hitting
                if tasks_prop:
                    if "melee" in tasks_prop:
                        return True
                return move_to_face + Operation("use",
                                                Operation("punch", Entity(me.entity.id, targets=[Entity(enemy.id)])))
        else:
            print("Out of reach. Reach is {} and distance is {}".format(reach, distance))
            # Check if we should stop hitting
            if tasks_prop:
                if "melee" in tasks_prop:
                    return Operation("use",
                                     Root(args=[Entity("stop")], id="melee", objtype="task"))

    def attack_ranged(self, me, enemy):
        print("attack ranged")
        # check that we can reach the target, and if so attack it
        distance = distance_between(me.entity.location, enemy.location)
        if distance is None:
            print("Could not calculate distance.")
            return
        attached_current = me.get_attached_entity("hand_primary")
        tasks_prop = me.entity.get_prop_map('tasks')

        if distance < 50:
            print("in range")
            move_to_face = me.face(enemy)

            if attached_current:
                # Check if we're already drawing
                if tasks_prop:
                    if "draw" in tasks_prop:
                        # Check if we can release
                        draw_task = tasks_prop["draw"]
                        # print("draw task {}".format(str(draw_task)))
                        usages = draw_task["usages"]
                        for usage in usages:
                            print(usage.name)
                            if usage.name == "release":
                                print("release bow")
                                direction = enemy.location.pos - me.entity.location.pos
                                direction.normalize()
                                return move_to_face + Operation("use",
                                                                Root(args=[Entity("release", direction=[direction])],
                                                                     id="draw",
                                                                     objtype="task"))

                        return True
                    print("Drawing bow")
                    direction = enemy.location.pos - me.entity.location.pos
                    direction.normalize()
                    return move_to_face + \
                           Operation("use", Operation("draw", Entity(attached_current.id, direction=[direction])))


class FightOrFlight(Goal):
    """Fight or flee from enemies in range"""

    def __init__(self, range=30):
        Goal.__init__(self, desc="fight or flee from enemies",
                      subgoals=[KeepGrudge(),
                                Fight(what="memory.disposition <= -1 && memory.threat <= 0", range=range),
                                Avoid(what="memory.disposition <= -1 && memory.threat > 0", range=range)])


class KeepGrudge(DynamicGoal):
    """React to other entities hitting us and lower their disposition."""

    def __init__(self):
        DynamicGoal.__init__(self, trigger="sight_hit", desc="React when being hit.")

    def event(self, me, original_op, op):
        # The args of the hit op contains the originating entity for the hit (i.e. the actual attacker).
        hit_args = op[0]
        actor_id = hit_args.id
        to_id = op.to
        print('Got sight of hit from "{}" to "{}"'.format(actor_id, to_id))
        if actor_id and to_id:
            # Check that it's not from ourselves
            if actor_id == me.entity.id:
                return
            # Ignore it it's us being hit
            if to_id != me.entity.id:
                return

            # Alter the base disposition for this entity
            disposition_base = me.map.recall_entity_memory(actor_id, "disposition_base", 0)
            # Alter it slightly. Here we could check how much damage it did and alter it depending on that
            me.map.add_entity_memory(actor_id, "disposition_base", disposition_base - 0.4)
            print("Updated base disposition of entity {} to {}.".format(actor_id, disposition_base - 0.4))
            if actor_id in me.entities:
                entity = me.entities[actor_id]
                me.update_relation_for_entity(entity)
