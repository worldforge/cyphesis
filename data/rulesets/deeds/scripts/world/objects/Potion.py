# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

import importlib

import server
from atlas import Operation, Entity, Oplist


def consume(instance):
    """ When drinking the potion the __effects should be applied to the drinker, and the potion destroyed.
     In addition, if a script handler is registered in the __handler property it will be called as well.
     """

    op_list = Oplist()
    op_list += Operation("delete", Entity(instance.tool.id), to=instance.tool)

    handler_props = instance.tool.props["__handler"]
    if handler_props is not None:
        mod_name, func_name = handler_props["name"].rsplit('.', 1)
        mod = importlib.import_module(mod_name)
        func = getattr(mod, func_name)
        result = func(instance)
        if result:
            op_list.append(result)

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
