# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

from atlas import Operation, Entity, Oplist
from world.utils import Usage

import server


def eat(instance):
    # Delete ourselves, and send a Nourish to the eater

    nourish_ent = Entity()

    # Check if there's a conversion rate for mass->biomass
    if instance.tool.props.mass and instance.tool.props._eat_mass_conversion:
        nourish_ent.mass = instance.tool.props.mass * instance.tool.props._eat_mass_conversion
    if instance.tool.props._consumable_type:
        nourish_ent.eat_type = instance.tool.props._consumable_type

    return server.OPERATION_BLOCKED, Operation("nourish", nourish_ent, to=instance.actor), Operation("delete", Entity(instance.tool.id), to=instance.tool)
