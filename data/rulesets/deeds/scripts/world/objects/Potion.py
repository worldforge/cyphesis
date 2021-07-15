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
        # Copy over all props in "__effects" to a "set" op.
        for prop_name, effect in effects_prop.items():
            ent = Entity(instance.actor.id)
            ent[str(prop_name)] = effect
            op_list += Operation("set", ent, to=instance.actor.id)

    msg_prop = instance.tool.props["__message"]
    if msg_prop is not None:
        op_list += Operation("imaginary", Entity(description=str(msg_prop)), to=instance.actor.id,
                             from_=instance.actor.id)
    op_list.append(instance.actor.start_action("drinking", 1))

    return server.OPERATION_BLOCKED, op_list
