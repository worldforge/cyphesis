#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Vector3D

import server

class Slaughter(server.Task):
    """ A task for killing livestock for food."""
    def cut_operation(self, op):
        """ Op handler for cut op which activates this task """
        # print "Slaughter.cut"

        if len(op) < 1:
            sys.stderr.write("Slaughter task has no target in cut op")

        # FIXME Use weak references, once we have them
        self.target = server.world.get_object_ref(op[0].id)
        self.tool = op.to

        self.count = 0

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Slaughter.tick"

        target = self.target()

        if target is None:
            # print "Target is no more"
            self.irrelevant()
            return

        if self.count == 0:
            self.count = int(target.mass)
            # print "setting target mass to ", self.count

        #Measure the distance between the entity horizontal edges. Else we won't be able to reach if either entity is too thick.
        char_loc = self.character.location
        target_loc = self.target().location
        entity_edges_dis = square_horizontal_edge_distance(char_loc, target_loc) 
        
        #Assume that a standard human can reach 1.5 meters, and use this to determine if we're close enough to be able to perform the logging
        standard_human_reach_squared=1.5*1.5

        if entity_edges_dis > standard_human_reach_squared:
            self.rate = 0
            # print "Too far away"
            return self.next_tick(1.75)

        res=Oplist()

        if target.mass <= 1:
            set = Operation("set", Entity(target.id, status = -1), to = target)
            res.append(set)
        else:
            set = Operation("set", Entity(target.id, mass = target.mass - 1),
                            to = target)
            res.append(set)

        if self.count < 1:
            self.progress = 1.0
            self.rate = 0
        else:
            self.progress = (self.count - target.mass) / self.count
            self.rate = (1.0 / self.count) / 1.75

        chunk_loc = target.location.copy()
        meat_type = 'meat'

        if hasattr(target, 'meat'):
            meat_type = target.meat

        chunk_loc.coordinates = target.location.coordinates

        chunk_loc.orientation = target.location.orientation

        create=Operation("create",
                         Entity(name = meat_type,
                                type = meat_type,
                                mass = 1,
                                location = chunk_loc), to = target)
        res.append(create)

        res.append(self.next_tick(1.75))

        return res
