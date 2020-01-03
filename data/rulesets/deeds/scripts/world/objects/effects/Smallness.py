# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2020 Erik Ogenvik (See the file COPYING for details).

from atlas import Operation, Entity
from rules import Location


def potion_handler(instance):
    """
     Create a smallness instance which disappears after 30 seconds, with scale being halved.
    :param instance:
    :return:
    """
    new_entity = Entity(parent="effect",
                        location=Location(instance.actor),
                        transient=30,
                        modify=[{"modifier": {"scale": {"default": [0.5]}}}])
    return Operation("create", new_entity, to=instance.tool.id)
