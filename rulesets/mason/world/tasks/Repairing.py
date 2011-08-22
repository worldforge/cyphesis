#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Vector3D

import server

class Repairing(server.Task):
    """A very simple Repair system for Repairing structures."""

    materials = ["wood"]

    def consume_materials (self) :
        for item in self.character.contains:
            if item.type[0] == str(self.materials[0]):
                set = Operation("set", Entity(item.id, status = -1), to = item)
                return set
        else : 
            print "No Wooden wall in inventory"
            return 0

    def repair_operation(self, op):
        """ The repair op is FROM the the character,
            TO the structure that is getting Repaired which we
            term the target. """

        if len(op) < 1:
            sys.stderr.write("Repair task has no target in repair op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

    def tick_operation(self, op):
        """ This method is called repeatedly, each time a Repair turn occurs.
            In this example the interval is fixed, but it can be varied. """
        # print "Repair.tick" 

        target=server.world.get_object(self.target)
        res=Oplist()
        current_status = 0
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        if self.character.stamina <= 0:
            # print "I am exhausted"
            self.irrelevant()
            return

        if square_distance(self.character.location, target.location) > target.location.bbox.square_bounding_radius():
            self.progress = current_status
            self.rate = 0
            return self.next_tick(1.75)

        if hasattr ( target, 'status' ) : 
            current_status = target.status
        else:
            set = Operation("set", Entity(self.target, status = 1),
                            to = self.target)
            res.append(set)
            current_status = 1.0
            self.irrelevant()

        if current_status < 0.9:
            set=Operation("set", Entity(self.target, status=current_status+0.1), to=self.target)
            res.append(set)
            consume =  self.consume_materials ()
            if consume : 
                res.append(consume)
            else : 
                self.irrelevant()

        else:
            set = Operation("set", Entity(self.target, status = 1),
                            to = self.target)
            res.append(set)
            self.irrelevant()

        self.progress = current_status
        self.rate = 0.1 / 1.75
        
        res.append(self.next_tick(1.75))

        return res

