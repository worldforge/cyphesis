# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

import server
from atlas import Operation, Entity


def consume(instance):
    # Delete ourselves, and send a Nourish to the eater

    nourish_ent = Entity()

    # Check if there's a conversion rate for mass->biomass
    if instance.tool.props.mass and instance.tool.props._consume_mass_conversion:
        nourish_ent.mass = instance.tool.props.mass * instance.tool.props._consume_mass_conversion
    if instance.tool.props.consumable_type:
        nourish_ent.consume_type = instance.tool.props.consumable_type

    # TODO: match with animation in client
    return server.OPERATION_BLOCKED, \
           Operation("nourish", nourish_ent, to=instance.actor), \
           Operation("delete", Entity(instance.tool.id), to=instance.tool), \
           instance.actor.start_action("eating", 1)
