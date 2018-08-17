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

        self.target = server.world.get_object(op[0].id)
        self.tool = op.to

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Logging.tick"

        if self.target() is None:
            # print "Target is no more"
            self.irrelevant()
            return

        current_status = self.target().status

        #Measure the distance between the entity horizontal edges. Else we won't be able to reach if either entity is too thick.
        distance_between_entity_edges_squared = square_horizontal_edge_distance(self.character.location, self.target().location) 
        
        #Assume that a standard human can reach 1.5 meters, and use this to determine if we're close enough to be able to perform the logging
        standard_human_reach_squared=1.5*1.5

        if distance_between_entity_edges_squared > standard_human_reach_squared:
            self.progress = 1 - current_status
            self.rate = 0
            return self.next_tick(1.75)

        res=Oplist()
        if current_status > 0.11:
            set=Operation("set", Entity(self.target().id, status=current_status-0.1), to=self.target())
            res.append(set)
            # print "CHOP",current_status

            normal=Vector3D(0, 1, 0)
            # print "LOC.ori ", self.target().location.orientation
            # calculate how tilted the tree is already
            if self.target().location.orientation.is_valid():
                normal.rotate(self.target().location.orientation)
            # print "Normal ", normal, normal.dot(Vector3D(0, 1, 0))
            # if the tree is standing, and it's already half cut down, rotate
            # it to be horizontal, away from the character
            if normal.dot(Vector3D(0, 1, 0)) > 0.8 and current_status < 0.5:
                # print "Fall down"
                # determine the axis of rotation by cross product of the vector
                # from character to tree, and vertically upward vector
                axis = distance_to(self.character.location,
                                   self.target().location).cross(Vector3D(0, 1, 0))
                # the axis must be a unit vector
                try:
                    axis = axis.unit_vector()
                except ZeroDivisionError:
                    axis = Vector3D(1,0,0)
                # print "axis ", axis
                # create a rotation of 90 degrees around this axis
                orient = Quaternion(axis, math.pi / -2.0)

                # if the tree is rotated, apply this too
                if self.target().location.orientation.is_valid():
                    orient = self.target().location.orientation * orient

                move_location = self.target().location.copy()
                move_location.orientation = orient

                move = Operation("move", Entity(self.target().id, mode='felled',
                                                location=move_location),
                                 to = self.target())
                res.append(move)
        else:
            # print "become log"
            set = Operation("set", Entity(self.target().id, status = -1),
                            to = self.target())
            res.append(set)
            create_loc = self.target().location.copy()
            create_loc.orientation = self.target().location.orientation
            create = Operation("create",
                               Entity(parent="lumber",
                                      mass = self.target().mass, 
                                      location = create_loc,
                                      bbox = self.target().bbox),
                               to = self.target())
            res.append(create)
                                            
        self.progress = 1 - current_status
        self.rate = 0.1 / 1.75
        
        res.append(self.next_tick(1.75))

        return res
