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
            new_status = self.props.status - 0.1
            res.append(Operation("set", Entity(self.id, status=new_status), to=self.id))

            damageProp = self.props.damage
            if self.location.parent and damageProp is not None:
                res.append(Operation('hit', Entity(hit_type="poison", id=self.id, damage=damageProp),
                                     to=self.location.parent.id))

            return server.OPERATION_HANDLED, res
        return server.OPERATION_IGNORED
