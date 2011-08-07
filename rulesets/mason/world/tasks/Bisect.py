#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).
#Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Vector3D

import server

class Bisect(server.Task):
    """ A task for cutting a section of material in two."""
    def bicut_operation(self, op):
        """ Op handler for cut op which activates this task """
        # print "Bisect.bicut"

        if len(op) < 1:
            sys.stderr.write("Bisect task has no target in cut op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to


    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Bisect.tick"

        target=server.world.get_object(self.target)
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        if square_distance(self.character.location, target.location) > target.location.bbox.square_bounding_radius():
            self.rate = 0
            # print "Too far away"
            return self.next_tick(0.75)
        else:
            self.progress += 1

        self.rate = 0.5 / 0.75

        if self.progress < 1:
            # print "Not done yet"
            return self.next_tick(0.75)

        length = target.location.bbox.far_point.z - target.location.bbox.near_point.z
        mid = length/2

        if mid <= 4:
            # print "Nothing more to cut"
            self.irrelevant()
            return

        res=Oplist()

        new_bbox = [target.location.bbox.near_point.x,
                    target.location.bbox.near_point.y,
                    target.location.bbox.near_point.z,
                    target.location.bbox.far_point.x ,
                    target.location.bbox.far_point.y,
                    target.location.bbox.far_point.z - mid ]

        set=Operation("set", Entity(target.id, bbox=new_bbox), to=target)
        res.append(set)

        slice_loc = target.location.copy()

        slice_loc.coordinates = target.location.coordinates

        slice_bbox = [target.location.bbox.near_point.x,
                      target.location.bbox.near_point.y,
                      0,
                      target.location.bbox.far_point.x,
                      target.location.bbox.far_point.y,
                      mid]
        slice_loc.orientation = target.location.orientation
        
        # create to convert remaining fragment into a wood board
        create=Operation("create", Entity(name='wood', type='wood', location=slice_loc, bbox=slice_bbox), to=target)
        res.append(create)

        if mid - 4 > 4:
            self.progress = 0

        res.append(self.next_tick(0.75))

        return res
