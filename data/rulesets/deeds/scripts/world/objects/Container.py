# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

import server
from atlas import Operation, Entity


def open_container(instance):
    """ Standard open function"""
    return server.OPERATION_BLOCKED, \
           Operation("set", Entity({"__container_access!append": [instance.actor.id]}), to=instance.tool.id)
