# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

import ai
import entity_filter
from atlas import Operation, Entity, Root

from mind.Goal import Goal
from mind.goals.common.common import get_reach
from mind.goals.common.move import Avoid, Condition, MoveMe
from mind.goals.dynamic.DynamicGoal import DynamicGoal

# A list of usages which we should look for in weapons.
weapon_usages = ['strike', 'chop']
unarmed_usages = ['punch', 'strike', 'chop']


class Fight(Goal):
    """Fight enemies in range."""

    def __init__(self, what="", range=30):
        Goal.__init__(self, desc="Fight something.",
                      sub_goals=[
                          Condition(condition_fn=self.has_enemy_in_range,
                                    goals_true=[
                                        Condition(condition_fn=self.should_use_melee,
                                                  goals_true=[
                                                      self.equip_melee_weapon,
                                                      self.equip_shield,
                                                      self.attack_melee,
                                                      MoveMe(location=self.get_enemy, radius=0, speed=0.5)
                                                  ],
                                                  goals_false=[
                                                      self.equip_ranged_weapon,
                                                      self.stop_moving,
                                                      self.attack_ranged,
                                                  ])],
                                    goals_false=[self.stop_attack_task])
                      ])
        self.what = what
        self.filter = entity_filter.Filter(what)
        self.range = range
        self.square_range = range * range
        self.vars = ["what", "range"]
        self.weapon_usage = None
        self.use_ranged = False
        self.closest_enemy = None
        self.distance_to_enemy = None

    def stop_moving(self, me):
        me.steering.set_destination()

    def stop_attack_task(self, me):
        tasks_prop = me.entity.get_prop_map('tasks')
        if tasks_prop:
            if "melee" in tasks_prop:
                return Operation("use", Root(args=[Entity("stop")], id="melee", objtype="task"))
            if "draw" in tasks_prop:
                return Operation("use", Root(args=[Entity("stop")], id="draw", objtype="task"))

        # if self.active_attack_task:
        #     print("stopping attack task")
        #     self.active_attack_task = None
        #     return Operation("use", Root(args=[Entity("stop")], id=self.active_attack_task, objtype="task"))

    def has_enemy_in_range(self, me):
        closest_distance = self.range
        closest_enemy = None
        thing_all = me.map.find_by_filter(self.filter)
        for thing in thing_all:
            distance = me.steering.distance_to(thing, ai.EDGE, ai.EDGE)
            if distance is not None and distance <= closest_distance:
                closest_distance = distance
                closest_enemy = thing

        self.closest_enemy = closest_enemy
        if self.closest_enemy:
            self.distance_to_enemy = closest_distance
            return True
        else:
            self.distance_to_enemy = None
            return False

    def should_use_melee(self, me):
        # If possible, shoot with ranged weapons if a bit away
        if self.distance_to_enemy > 5:
            # Check that we have bow and arrows
            # First check if we have any arrows
            has_arrows = False
            for child in me.entity.contains:
                if child.is_type("arrow"):
                    has_arrows = True
                    break

            if not has_arrows:
                return True

            for child in me.entity.contains:
                if child.is_type("bow"):
                    return False
        return True

    def equip_shield(self, me):
        # First check if we're holding a shield
        attached_current = me.get_attached_entity("hand_secondary")
        if attached_current:
            # Check that the attached entity is a shield
            if attached_current.is_type("shield"):
                return None
        # Current tool isn't a shield, or we have nothing attached, try to find one.
        for child in me.entity.contains:
            if child.is_type("shield"):
                return Operation("wield", Entity(child.id, attachment="hand_secondary"))
        return None

    def equip_ranged_weapon(self, me):
        attached_current = me.get_attached_entity("hand_primary")
        if attached_current:
            # Check that the attached entity is a bow
            if attached_current.is_type("bow"):
                self.use_ranged = True
                return None
        # Current tool isn't a bow, or we have nothing attached, try to find one.
        for child in me.entity.contains:
            if child.is_type("bow"):
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

    def get_enemy(self, me):
        return self.closest_enemy

    def attack_melee(self, me):
        enemy = self.get_enemy(me)

        reach = get_reach(me)
        attached_current = me.get_attached_entity("hand_primary")
        tasks_prop = me.entity.get_prop_map('tasks')
        if self.distance_to_enemy - reach <= 0:
            move_to_face = me.face(enemy)

            # Check if we're already hitting
            if tasks_prop:
                if "melee" in tasks_prop:
                    return True

            if attached_current:
                return move_to_face + Operation("use", Operation(self.weapon_usage, Entity(attached_current.id,
                                                                                           targets=[Entity(enemy.id)])))
            else:
                # See if we have an unarmed action which we can use to strike
                own_usages = me.entity.get_prop_map("_usages")
                if own_usages:
                    for usage, _ in own_usages.items():
                        if usage in unarmed_usages:
                            op = Operation(usage, Entity(me.entity.id, targets=[Entity(enemy.id)]))
                            return move_to_face + Operation("use", op)
                print("Could not find any unarmed combat usages for this entity.")
        else:
            # print("Out of reach. Reach is {} and distance is {:.2f}m".format(reach, self.distance_to_enemy))
            # Check if we should stop hitting
            if tasks_prop:
                if "melee" in tasks_prop:
                    return Operation("use",
                                     Root(args=[Entity("stop")], id="melee", objtype="task"))

    def attack_ranged(self, me):
        enemy = self.get_enemy(me)
        attached_current = me.get_attached_entity("hand_primary")
        tasks_prop = me.entity.get_prop_map('tasks')

        if self.distance_to_enemy < 50:
            move_to_face = me.face(enemy)

            if attached_current:
                # Check if we're already drawing
                if tasks_prop and "draw" in tasks_prop:
                    # Check if we can release
                    draw_task = tasks_prop["draw"]
                    # print("draw task {}".format(str(draw_task)))
                    usages = draw_task["usages"]
                    for usage in usages:
                        if usage.name == "release":
                            direction = (enemy.location.pos + enemy.location.bbox.center) - \
                                        (me.entity.location.pos + me.entity.location.bbox.center)
                            direction.normalize()
                            return move_to_face + Operation("use",
                                                            Root(args=[Entity("release", direction=[direction])],
                                                                 id="draw",
                                                                 objtype="task"))

                    return True
                direction = enemy.location.pos - me.entity.location.pos
                direction.normalize()
                op = Operation("use", Operation("draw", Entity(attached_current.id, direction=[direction])))
                return move_to_face + op


class FightOrFlight(Goal):
    """Fight or flee from enemies in range"""

    def __init__(self, range=30):
        Goal.__init__(self, desc="fight or flee from enemies",
                      sub_goals=[KeepGrudge(),
                                 Fight(what="memory.disposition <= -1 && memory.threat <= 0", range=range),
                                 Avoid(what="memory.disposition <= -1 && memory.threat > 0", range=range)])


class ClearGrudge(DynamicGoal):
    """
    A dynamic goal which clears any entity specific when an entity is deleted or defeated.
    This is useful to avoid NPC being hostile to players even after they have killed the player.
    """

    def __init__(self):
        DynamicGoal.__init__(self, trigger=["sight_defeated", "sight_delete"],
                             desc="Clear grudge data when entity defeated or deleted.")

    def event(self, me, original_op, op):
        inner_args = op[0]
        entity_id = inner_args.id
        # Check that it's not from ourselves
        if entity_id == me.entity.id:
            return

        me.map.remove_entity_memory(entity_id, "disposition_base")
        # print("Updated base disposition of entity {} to {}.".format(actor_id, disposition_base - 0.4))
        if entity_id in me.entities:
            entity = me.entities[entity_id]
            me.update_relation_for_entity(entity)


class KeepGrudge(DynamicGoal):
    """React to other entities hitting us and lower their disposition."""

    def __init__(self):
        DynamicGoal.__init__(self, trigger="sight_hit", desc="React when being hit.")
        self.clear_goal = ClearGrudge()

    def triggering_goals(self):
        return [self, self.clear_goal]

    def event(self, me, original_op, op):
        # The args of the hit op contains the originating entity for the hit (i.e. the actual attacker).
        hit_args = op[0]
        actor_id = hit_args.id
        to_id = op.to
        # print('Got sight of hit from "{}" to "{}"'.format(actor_id, to_id))
        if actor_id and to_id:
            # Check that it's not from ourselves
            if actor_id == me.entity.id:
                return
            # Ignore if it's not us being hit
            if to_id != me.entity.id:
                return

            # Alter the base disposition for this entity
            disposition_base = me.map.recall_entity_memory(actor_id, "disposition_base", 0)
            # Alter it slightly. Here we could check how much damage it did and alter it depending on that
            me.map.add_entity_memory(actor_id, "disposition_base", disposition_base - 0.4)
            # print("Updated base disposition of entity {} to {}.".format(actor_id, disposition_base - 0.4))
            if actor_id in me.entities:
                entity = me.entities[actor_id]
                me.update_relation_for_entity(entity)
