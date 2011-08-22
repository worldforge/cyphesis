#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Vector3D

import server

class Destroying(server.Task):
    """A very simple Destroy system for destroying structures."""
    def attack_operation(self, op):
        """ The attack op is FROM the the character,
            TO the structure that is getting destroyed which we
            term the target. """

        if len(op) < 1:
            sys.stderr.write("Destroy task has no target in attack op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

    def tick_operation(self, op):
        """ This method is called repeatedly, each time a Destroy turn occurs.
            In this example the interval is fixed, but it can be varied. """
        # print "Destroy.tick" 

        target=server.world.get_object(self.target)
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        if self.character.stamina <= 0:
            # print "I am exhausted"
            self.irrelevant()
            return

        res=Oplist()
        chunk_loc = target.location.copy()
        chunk_loc.coordinates = target.location.coordinates
        chunk_loc.orientation = target.location.orientation

        if hasattr ( target, 'status' ) : 
            current_status = target.status

        else:
            set = Operation("set", Entity(self.target, status = 1),
                            to = self.target)
            res.append(set)
            current_status = 1.0

        if square_distance(self.character.location, target.location) > target.location.bbox.square_bounding_radius():
            self.progress = 1 - current_status
            self.rate = 0
            return self.next_tick(1.75)

        if current_status > 0.11:
            set=Operation("set", Entity(self.target, status=current_status-0.1), to=self.target)
            res.append(set)

        else:
            if target.type[0] == "castle_house_a" :
                create=Operation("create", Entity(name = "castle_house_ruin", type = "castle_house_ruin", location = chunk_loc), to = target)
                res.append(create)

            set = Operation("set", Entity(self.target, status = -1),
                            to = self.target)
            res.append(set)
            self.irrelevant()

        self.progress = 1 - current_status
        self.rate = 0.1 / 1.75
        
        res.append(self.next_tick(1.75))

        return res

