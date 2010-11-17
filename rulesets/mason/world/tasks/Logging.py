#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Vector3D

import math

from random import *

import server

class Logging(server.Task):
    """ A proof of concept task for logging."""
    def cut_operation(self, op):
        """ Op handler for cut op which activates this task """
        # print "Logging.cut"

        if len(op) < 1:
            sys.stderr.write("Logging task has no target in cut op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Logging.tick"

        target=server.world.get_object(self.target)
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        current_status = target.status

        if square_distance(self.character.location, target.location) > target.location.bbox.square_bounding_radius():
            self.progress = 1 - current_status
            self.rate = 0
            return self.next_tick(1.75)

        res=Oplist()
        if current_status > 0.11:
            set=Operation("set", Entity(self.target, status=current_status-0.1), to=self.target)
            res.append(set)
            # print "CHOP",current_status

            normal=Vector3D(0,0,1)
            # print "LOC.ori ", target.location.orientation
            if target.location.orientation.is_valid():
                normal.rotate(target.location.orientation)
            # print "Normal ", normal, normal.dot(Vector3D(0,0,1))
            if normal.dot(Vector3D(0,0,1)) > 0.8 and current_status < 0.5:
                # print "Fall down"
                axis = Vector3D(uniform(-1,1), uniform(-1,1), 0)
                axis = axis.unit_vector()
                orient = target.location.orientation
                orient.rotation(axis, math.pi / 2)

                move_location = target.location.copy()
                move_location.orientation = orient

                move = Operation("move", Entity(self.target, mode='felled',
                                                location=move_location),
                                 to = self.target)
                res.append(move)
        else:
            # print "become log"
            set = Operation("set", Entity(self.target, status = -1),
                            to = self.target)
            res.append(set)
            create_loc = target.location.copy()
            create_loc.orientation = target.location.orientation
            create = Operation("create",
                               Entity(parents = ["lumber"],
                                      mass = target.mass, 
                                      location = create_loc,
                                      bbox = target.bbox),
                               to = self.target)
            res.append(create)
                                            
        self.progress = 1 - current_status
        self.rate = 0.1 / 1.75
        
        res.append(self.next_tick(1.75))

        return res
