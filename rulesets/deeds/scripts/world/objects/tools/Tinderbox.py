#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005 Al Riddoch (See the file COPYING for details).

from atlas import *

from physics import Point3D

import server

def ignite(tool, actor, op, targets, consumed):
    target = targets[0]
    if target[0].is_reachable_for_other_entity(actor, target[1], 0):
        return (server.OPERATION_BLOCKED, Operation("create", Entity(parent='fire',status=0.05, location=Location(target[0],target[1])),to=target[0]))
    else:
        print("Too far away.")
        return (server.OPERATION_BLOCKED)
