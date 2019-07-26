# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

from atlas import Operation, Entity, Oplist
from world.utils import Usage

import server


def consume(instance):
    """ When drinking the potion the __effects should be applied to the drinker, and the potion destroyed """

    op_list = Oplist()
    op_list += Operation("delete", Entity(instance.tool.id), to=instance.tool)

    effects_prop = instance.tool.props["__effects"]
    if effects_prop is not None:
        for prop_name, effect in effects_prop.items():
            actual_value = effect
            effect_prop = instance.actor.props[prop_name]
            if effect_prop is not None:
                actual_value = effect_prop + effect
            ent = Entity(instance.actor.id)
            ent[str(prop_name)] = actual_value
            op_list += Operation("set", ent, to=instance.actor.id)

    msg_prop = instance.tool.props["__message"]
    if msg_prop is not None:
        op_list += Operation("imaginary", Entity(description=str(msg_prop)), to=instance.actor.id,
                             from_=instance.actor.id)

    return server.OPERATION_BLOCKED, op_list
