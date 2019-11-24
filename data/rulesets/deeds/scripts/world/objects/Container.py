# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

from atlas import Operation, Entity, Oplist
from world.utils import Usage

import server


def open_container(instance):

    return server.OPERATION_BLOCKED
