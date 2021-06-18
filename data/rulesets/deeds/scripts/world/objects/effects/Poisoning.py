# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

import server
from atlas import Operation, Entity, Oplist
from rules import Location

from world.utils import Ticks


def potion_handler(instance):
    """
     Create a poisoning instance
    :param instance:
    :return:
    """
    new_entity = Entity(parent="poisoning",
                        location=Location(instance.actor))
    return Operation("create", new_entity, to=instance.tool.id)


class Poisoning(server.Thing):
    """
    Used for entities that poison their parent. Each tick a "poison" hit will be sent to the parent.
    """

    tick_interval = 10

    def __init__(self, cpp):
        Ticks.init_ticks(self, self.tick_interval)

    def tick_operation(self, op):
        res = Oplist()
        if Ticks.verify_tick(self, op, res, self.tick_interval):
            # Make ourselves go away after ten ticks by decreasing our status.
            res.append(Operation("set", Entity(self.id, {"status!subtract": 0.1}), to=self.id))

            damage_prop = self.props.damage
            if self.parent and damage_prop is not None:
                # If there's an "entity_ref" prop it's the reference to the actor which caused the poisoning.
                actor_id = self.id
                entity_ref_prop = self.props.entity_ref
                if entity_ref_prop is not None:
                    actor_id = entity_ref_prop["$eid"]

                res.append(Operation('hit', Entity(hit_type="poison", id=actor_id, damage=damage_prop),
                                     to=self.parent.id))

            return server.OPERATION_HANDLED, res
        return server.OPERATION_IGNORED
